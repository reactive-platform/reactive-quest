#pragma once

// States are owned by shared_ptr, which stands in for the C# GC: a derived state holds its sources, and a binding holds the state it observes.

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "reactive/ReactiveScheduler.hpp"

namespace reactive {
class IStateBase;
template <class T> class IState;
template <class T> struct StateBinding;
template <class TOut, class... TIn> class DerivedState;
template <class T> class BranchedState;

class StateSubscription {
public:
    StateSubscription() = default;
    StateSubscription(std::weak_ptr<IStateBase> state, uint32_t id) : _state(std::move(state)), _id(id) {}

    uint32_t Id() const { return _id; }

    inline void RemoveCallback();

private:
    std::weak_ptr<IStateBase> _state;
    uint32_t                  _id{0};
};

class IStateBase : public std::enable_shared_from_this<IStateBase> {
public:
    IStateBase() = default;
    IStateBase(IStateBase const&)            = delete;
    IStateBase& operator=(IStateBase const&) = delete;
    virtual ~IStateBase() = default;

    virtual void Flush() = 0;
    virtual StateSubscription AddInvalidationCallback(std::function<void()> callback) = 0;
    virtual void RemoveCallback(StateSubscription const& subscription) = 0;
    virtual void ClearCallbacks() = 0;
};

inline void StateSubscription::RemoveCallback() {
    if (auto state = _state.lock()) state->RemoveCallback(*this);
    _state.reset();
    _id = 0;
}

template <class S>
concept StateType = requires { typename S::value_type; } &&
                    std::derived_from<S, IState<typename S::value_type>>;

template <class T>
class IState : public IStateBase {
public:
    using value_type = T;

    virtual T const& Value() const = 0;
    virtual StateSubscription AddValueCallback(std::function<void(T const&)> callback) = 0;

    std::shared_ptr<IState<T>> SharedState() {
        return std::static_pointer_cast<IState<T>>(this->shared_from_this());
    }

    StateSubscription AddCallback(std::function<void(T const&)> callback, bool lazy = false) {
        auto subscription = AddValueCallback(callback);
        if (!lazy) callback(Value());
        return subscription;
    }

    template <class F>
    auto Map(F&& fn);

    template <class F>
    std::shared_ptr<BranchedState<T>> Where(F&& predicate);

    // A lazy binding doesn't push the current value when bound, only later changes.
    StateBinding<T> In();
    StateBinding<T> InLazy();
};

// Defined in libreactive, not inline: StateBase<T> is also instantiated in consumer mods, and each .so would get its own copy.
struct StateInvalidation {
    static int      Depth;
    static uint64_t Walk;
};

template <class T>
class StateBase : public IState<T> {
public:
    void Flush() override {
        if (!_dirty) return;
        _dirty = false;
        OnFlush();
        if (_valueCount == 0) return;
        // Passes a copy where possible, so a callback that changes the state can't pull the value out from under the rest.
        if constexpr (std::is_copy_constructible_v<T>) {
            T value = this->Value();
            InvokeValue(value);
        } else {
            InvokeValue(this->Value());
        }
    }

    StateSubscription AddValueCallback(std::function<void(T const&)> callback) override {
        auto& entry = AddEntry();
        entry.value = std::move(callback);
        ++_valueCount;
        return StateSubscription(this->weak_from_this(), entry.id);
    }

    StateSubscription AddInvalidationCallback(std::function<void()> callback) override {
        auto& entry        = AddEntry();
        entry.invalidation = std::move(callback);
        ++_invalidationCount;
        return StateSubscription(this->weak_from_this(), entry.id);
    }

    void RemoveCallback(StateSubscription const& subscription) override {
        for (auto& entry : _entries) {
            if (entry->id == subscription.Id() && !entry->removed) {
                Release(*entry);
                Compact();
                return;
            }
        }
    }

    void ClearCallbacks() override {
        for (auto& entry : _entries) {
            if (!entry->removed) Release(*entry);
        }
        Compact();
    }

protected:
    virtual void OnFlush() {}

    void NotifyValueInvalidated() {
        // A change reaches a state once for every path to it, but the state
        // only has to pass it on once. Otherwise stacked diamonds double the
        // work with each level.
        if (StateInvalidation::Depth == 0) {
            ++StateInvalidation::Walk;
        } else if (_invalidatedIn == StateInvalidation::Walk) {
            return;
        }
        _invalidatedIn = StateInvalidation::Walk;

        if (_invalidationCount > 0) {
            // Unwinds on throw. A depth left above zero would make every later
            // change look like part of this walk and vanish.
            struct DepthScope {
                DepthScope() { ++StateInvalidation::Depth; }
                ~DepthScope() { --StateInvalidation::Depth; }
            } depthScope;
            Invoke([](Entry& entry) {
                if (entry.invalidation) entry.invalidation();
            });
        }

        if (_valueCount > 0 && !_dirty) {
            _dirty = true;
            ReactiveScheduler::ScheduleState(this);
        }
    }

private:
    struct Entry {
        uint32_t                      id{0};
        bool                          removed{false};
        std::function<void(T const&)> value;
        std::function<void()>         invalidation;
    };

    // Pointers, because adding a callback mid-flush mustn't move a running one.
    std::vector<std::unique_ptr<Entry>> _entries;
    uint32_t _nextId{1};
    int      _valueCount{0};
    int      _invalidationCount{0};
    int      _iterating{0};
    bool     _dirty{false};
    uint64_t _invalidatedIn{0};

    Entry& AddEntry() {
        _entries.push_back(std::make_unique<Entry>());
        _entries.back()->id = _nextId++;
        return *_entries.back();
    }

    void Release(Entry& entry) {
        entry.removed = true;
        if (entry.value) {
            --_valueCount;
        } else {
            --_invalidationCount;
        }
    }

    void Compact() {
        if (_iterating > 0) return;
        std::erase_if(_entries, [](std::unique_ptr<Entry> const& entry) { return entry->removed; });
    }

    template <class F>
    void Invoke(F const& call) {
        // Unwinds on throw, or removed callbacks would never be released.
        struct IterationScope {
            StateBase& state;
            explicit IterationScope(StateBase& s) : state(s) { ++state._iterating; }
            ~IterationScope() {
                --state._iterating;
                state.Compact();
            }
        } scope(*this);

        const size_t count = _entries.size();  // callbacks added mid-flush wait for the next one
        for (size_t i = 0; i < count; ++i) {
            auto& entry = *_entries[i];
            if (!entry.removed) call(entry);
        }
    }

    void InvokeValue(T const& value) {
        Invoke([&value](Entry& entry) {
            if (entry.value) entry.value(value);
        });
    }
};

namespace detail {
template <class T> struct is_shared_ptr : std::false_type {};
template <class T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

// Types with cheap, well-defined equality skip no-op sets. Everything else always notifies, since PC's model classes compare by reference.
template <class T>
inline constexpr bool kStateSkipsEqual =
    std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_pointer_v<T> ||
    std::is_same_v<T, std::string> || is_shared_ptr<T>::value;
}

template <class T>
class State : public StateBase<T> {
public:
    explicit State(T initial) : _value(std::move(initial)) {}

    T const& Value() const override { return _value; }

    void SetValue(T value) {
        if constexpr (detail::kStateSkipsEqual<T>) {
            if (value == _value) return;
        }
        _value = std::move(value);
        this->NotifyValueInvalidated();
    }

    void EnforceInvalidation() { this->NotifyValueInvalidated(); }

private:
    T _value;
};

template <class TOut, class... TIn>
class DerivedState final : public StateBase<TOut> {
public:
    using Fn = std::function<TOut(TIn const&...)>;

    explicit DerivedState(Fn fn, std::shared_ptr<IState<TIn>>... states)
        : _fn(std::move(fn)), _states(std::move(states)...) {
        std::apply(
            [this](auto&... state) {
                size_t i = 0;
                ((_subscriptions[i++] = state->AddInvalidationCallback([this] {
                      _hasValue = false;
                      this->NotifyValueInvalidated();
                  })),
                 ...);
            },
            _states);
    }

    ~DerivedState() override {
        for (auto& subscription : _subscriptions) subscription.RemoveCallback();
    }

    TOut const& Value() const override {
        if (!_hasValue) {
            auto value = std::apply([this](auto const&... state) { return _fn(state->Value()...); }, _states);
            // Assign rather than reset+emplace so references handed out earlier keep pointing at live storage.
            if (_cache) {
                *_cache = std::move(value);
            } else {
                _cache.emplace(std::move(value));
            }
            _hasValue = true;
        }
        return *_cache;
    }

private:
    Fn                                            _fn;
    std::tuple<std::shared_ptr<IState<TIn>>...>   _states;
    std::array<StateSubscription, sizeof...(TIn)> _subscriptions{};
    mutable std::optional<TOut>                   _cache;
    mutable bool                                  _hasValue{false};
};

template <class T>
template <class F>
auto IState<T>::Map(F&& fn) {
    using TOut = std::decay_t<std::invoke_result_t<F&, T const&>>;
    return std::make_shared<DerivedState<TOut, T>>(std::function<TOut(T const&)>(std::forward<F>(fn)), SharedState());
}

namespace detail {
template <class S>
using state_value_t = typename std::remove_cvref_t<S>::element_type::value_type;

template <class Tuple, size_t... I>
auto MakeDerived(Tuple& args, std::index_sequence<I...>) {
    constexpr size_t N = sizeof...(I);
    using FnT  = std::remove_cvref_t<std::tuple_element_t<N, Tuple>>;
    using TOut = std::decay_t<std::invoke_result_t<FnT&, state_value_t<std::tuple_element_t<I, Tuple>> const&...>>;
    using Derived = DerivedState<TOut, state_value_t<std::tuple_element_t<I, Tuple>>...>;
    return std::make_shared<Derived>(
        typename Derived::Fn(std::get<N>(args)),
        std::shared_ptr<IState<state_value_t<std::tuple_element_t<I, Tuple>>>>(std::get<I>(args))...);
}
}

template <class... A>
auto RememberDerived(A&&... args) {
    static_assert(sizeof...(A) >= 2, "RememberDerived needs at least one state and a predicate");
    auto tuple = std::forward_as_tuple(std::forward<A>(args)...);
    return detail::MakeDerived(tuple, std::make_index_sequence<sizeof...(A) - 1>{});
}

template <class T>
class BranchedState final : public StateBase<T> {
public:
    BranchedState(std::shared_ptr<IState<T>> source, std::function<bool(T const&)> predicate)
        : _source(std::move(source)), _predicate(std::move(predicate)), _value(_source->Value()) {
        _subscription = _source->AddValueCallback([this](T const& value) {
            if (_predicate(value)) {
                _value = value;
                this->NotifyValueInvalidated();
            }
        });
    }

    ~BranchedState() override { _subscription.RemoveCallback(); }

    T const& Value() const override { return _value; }

private:
    std::shared_ptr<IState<T>>    _source;
    std::function<bool(T const&)> _predicate;
    T                             _value;
    StateSubscription             _subscription;
};

template <class T>
template <class F>
std::shared_ptr<BranchedState<T>> IState<T>::Where(F&& predicate) {
    return std::make_shared<BranchedState<T>>(SharedState(), std::function<bool(T const&)>(std::forward<F>(predicate)));
}

template <class T>
struct StateBinding {
    std::shared_ptr<IState<T>> state;
    bool                       lazy{false};

    StateBinding() = default;
    StateBinding(std::shared_ptr<IState<T>> s, bool isLazy) : state(std::move(s)), lazy(isLazy) {}

    template <StateType S>
        requires std::convertible_to<typename S::value_type, T>
    StateBinding(std::shared_ptr<S> s) {
        if (!s) return;
        if constexpr (std::is_same_v<typename S::value_type, T>) {
            state = std::move(s);
        } else {
            state = s->Map([](typename S::value_type const& v) -> T { return T(v); });
        }
    }

    template <class U>
        requires(!std::same_as<U, T> && std::convertible_to<U, T>)
    StateBinding(StateBinding<U> other) : lazy(other.lazy) {
        if (other.state) state = other.state->Map([](U const& v) -> T { return T(v); });
    }

    explicit operator bool() const { return static_cast<bool>(state); }
};

template <class T>
StateBinding<T> IState<T>::In() {
    return StateBinding<T>(SharedState(), false);
}

template <class T>
StateBinding<T> IState<T>::InLazy() {
    return StateBinding<T>(SharedState(), true);
}
}
