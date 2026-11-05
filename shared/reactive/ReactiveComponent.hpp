#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "custom-types/shared/coroutine.hpp"

#include "reactive/IReactiveComponent.hpp"
#include "reactive/animations/AnimatedState.hpp"
#include "reactive/animations/AnimationCurve.hpp"
#include "reactive/animations/AnimationDuration.hpp"
#include "reactive/animations/Interpolators.hpp"
#include "reactive/animations/Modules.hpp"
#include "reactive/layout/Props.hpp"
#include "reactive/state/State.hpp"

namespace reactive {
class YogaModifier;
struct PointerEventCallbacks;

template <class P>
concept HasItemProps = requires(P const& p) {
    p.flexItem;
    p.enabled;
    p.sEnabled;
    p.withinLayoutIfDisabled;
    p.name;
};

// C++ base constructors can't call the derived Construct(), so content is built on first need. Setters stash their values until then.
class ReactiveComponent : public IReactiveComponent {
public:
    ReactiveComponent();

    template <HasItemProps P>
    explicit ReactiveComponent(P const& props) : ReactiveComponent() {
        ApplyItemProps(props);
    }

    ~ReactiveComponent() override;

    ReactiveComponent(const ReactiveComponent&)            = delete;
    ReactiveComponent& operator=(const ReactiveComponent&) = delete;

    UnityEngine::GameObject*    Content() override;
    UnityEngine::RectTransform* ContentTransform() override;

    bool IsInitialized() const override { return _isInitialized; }
    bool GetEnabled()    const override;
    void SetEnabled(bool v) override;

    // Expires when the component is destroyed. Capture it in callbacks that can outlive the component.
    std::weak_ptr<void> LifetimeToken() const { return _lifetime; }

    void SetName(std::string name);

    void SetWithinLayoutIfDisabled(bool v);

    UnityEngine::GameObject* Use(UnityEngine::Transform* parent) override;
    UnityEngine::GameObject* Use() { return Use(nullptr); }

    // Call at the top of public methods that touch things created in Construct().
    void EnsureInitialized();

    using LayoutAppliedCallback = std::function<void(ReactiveComponent&)>;
    void AddLayoutAppliedListener(LayoutAppliedCallback cb) { _layoutAppliedListeners.push_back(std::move(cb)); }

    void RecalculateLayoutImmediate() override;
    void ScheduleLayoutRecalculation() override;

    YogaModifier& FlexItem();

    void ApplyFlexItem(FlexItemProps const& props);

    template <class Self, class F>
    Self* With(this Self& self, F&& fn) {
        self.EnsureInitialized();
        std::forward<F>(fn)(self);
        return &self;
    }

    template <class Self, class U>
        requires std::convertible_to<Self*, U*>
    Self* Bind(this Self& self, U*& out) {
        out = &self;
        return &self;
    }

    template <class TComp, class Self>
    Self* WithNativeComponent(this Self& self, TComp*& out) {
        out = self.Content()->template AddComponent<TComp*>();
        return &self;
    }

    template <class Self, StateType S, class F>
    Self* On(this Self& self, std::shared_ptr<S> const& state, F&& fn, bool lazy = false) {
        using T = typename S::value_type;
        Self* comp = &self;
        std::function<void(T const&)> apply = [comp, fn = std::forward<F>(fn)](T const& v) mutable {
            if constexpr (std::is_invocable_v<decltype(fn)&, Self&, T const&>) {
                fn(*comp, v);
            } else {
                fn(v);
            }
        };
        if (!lazy) self.EnsureInitialized();
        self.BindState(std::shared_ptr<IState<T>>(state), std::move(apply), lazy);
        return &self;
    }

    // Bodies are in reactive/layout/LayoutExtensions.hpp, which callers must include.
    template <class Self>
    Self* AsFlexItem(this Self& self, FlexItemProps const& props = {});

    // Bodies are in reactive/components/ComponentExtensions.hpp, which callers must include.
    template <class Self>
    Self* WithPointerEvents(this Self& self, PointerEventCallbacks const& callbacks);

    template <class Self>
    Self* AsRectMask(this Self& self, bool filterRaycasts = true);

    template <class Self, StateType S>
    Self* WithAlpha(this Self& self, std::shared_ptr<S> const& alpha, bool invert = false);

    template <class Self>
    Self* WithRectExpand(this Self& self) {
        auto* rect = self.ContentTransform();
        rect->set_anchorMin({0, 0});
        rect->set_anchorMax({1, 1});
        rect->set_sizeDelta({0, 0});
        rect->set_anchoredPosition({0, 0});
        return &self;
    }

    // Returned states are also kept alive until the component is destroyed, which then drops their callbacks.
    template <class T>
    std::shared_ptr<State<T>> Remember(T initialValue) {
        auto s = std::make_shared<State<T>>(std::move(initialValue));
        _ownedStates.push_back(s);
        return s;
    }

    template <class... A>
    auto RememberDerived(A&&... args) {
        auto s = ::reactive::RememberDerived(std::forward<A>(args)...);
        _ownedStates.push_back(s);
        return s;
    }

    template <class T>
    std::shared_ptr<AnimatedState<T>> RememberAnimated(
        T initialValue,
        IValueInterpolator<T>* interpolator,
        AnimationDuration duration,
        AnimationCurve* curve = nullptr,
        std::function<void(T const&)> onFinish = {}) {
        auto s      = std::make_shared<AnimatedState<T>>(std::move(initialValue), interpolator);
        s->Duration = duration;
        s->Curve    = curve ? curve : AnimationCurve::Linear();
        s->OnFinish = std::move(onFinish);
        _ownedStates.push_back(s);
        _modules.push_back(s.get());
        return s;
    }

    std::shared_ptr<AnimatedState<float>> RememberAnimated(
        float v, AnimationDuration d, AnimationCurve* c = nullptr, std::function<void(float const&)> onFinish = {}) {
        return RememberAnimated<float>(v, SingleValueInterpolator::Instance(), d, c, std::move(onFinish));
    }
    std::shared_ptr<AnimatedState<UnityEngine::Color>> RememberAnimated(
        UnityEngine::Color v, AnimationDuration d, AnimationCurve* c = nullptr,
        std::function<void(UnityEngine::Color const&)> onFinish = {}) {
        return RememberAnimated<UnityEngine::Color>(v, ColorValueInterpolator::Instance(), d, c, std::move(onFinish));
    }
    std::shared_ptr<AnimatedState<UnityEngine::Vector2>> RememberAnimated(
        UnityEngine::Vector2 v, AnimationDuration d, AnimationCurve* c = nullptr,
        std::function<void(UnityEngine::Vector2 const&)> onFinish = {}) {
        return RememberAnimated<UnityEngine::Vector2>(v, Vector2ValueInterpolator::Instance(), d, c, std::move(onFinish));
    }
    std::shared_ptr<AnimatedState<UnityEngine::Vector3>> RememberAnimated(
        UnityEngine::Vector3 v, AnimationDuration d, AnimationCurve* c = nullptr,
        std::function<void(UnityEngine::Vector3 const&)> onFinish = {}) {
        return RememberAnimated<UnityEngine::Vector3>(v, Vector3ValueInterpolator::Instance(), d, c, std::move(onFinish));
    }

    template <class T>
    void BindState(std::shared_ptr<IState<T>> state, std::function<void(T const&)> apply, bool lazy = false) {
        if (!state) return;
        auto subscription = state->AddCallback(std::move(apply), lazy);
        _bindings.push_back({std::move(state), subscription});
    }

    template <class T, class F>
    void BindProp(std::optional<T> const& value, StateBinding<T> const& binding, F&& apply) {
        if (value) apply(*value);
        if (binding) BindState<T>(binding.state, std::function<void(T const&)>(std::forward<F>(apply)), binding.lazy);
    }

    void StartCoroutine(custom_types::Helpers::Coroutine coroutine);
    void StopAllCoroutines();

    HostState* GetHostState() const override { return _host; }

protected:
    virtual UnityEngine::GameObject* Construct();
    virtual void Construct(UnityEngine::RectTransform* rect) {}
    virtual void OnInitialize() {}
    virtual void OnUpdate() {}
    virtual void OnStart() {}
    virtual void OnDestroyHook() {}
    virtual void OnEnable() {}
    virtual void OnDisable() {}
    virtual void OnRectDimensionsChanged() {}
    virtual void OnRecalculateLayoutSelf() {}
    virtual void OnLayoutApply() {}

    // For a component that measures itself: its size may have changed.
    void NotifyLayoutDataUpdated();

    template <HasItemProps P>
    void ApplyItemProps(P const& p) {
        ApplyFlexItem(p.flexItem);
        BindProp(p.enabled, p.sEnabled, [this](bool v) { SetEnabled(v); });
        if (p.withinLayoutIfDisabled) SetWithinLayoutIfDisabled(*p.withinLayoutIfDisabled);
        if (p.name) SetName(*p.name);
    }

private:
    void ConstructAndInit();

    void UpdateModules();
    void DispatchLayoutApplied();

    bool _isInitialized{false};
    bool _isConstructing{false};

    std::shared_ptr<void> _lifetime{std::make_shared<char>()};

    UnityEngine::GameObject*    _content{nullptr};
    UnityEngine::RectTransform* _contentTransform{nullptr};

    HostState* _host{nullptr};

    std::vector<FlexItemProps> _pendingFlexItems;
    std::optional<bool>        _pendingEnabled;
    std::optional<bool>        _pendingWithinLayoutIfDisabled;
    std::optional<std::string> _pendingName;

    std::vector<IReactiveModule*> _modules;
    std::vector<std::shared_ptr<IStateBase>> _ownedStates;

    struct Binding {
        std::shared_ptr<IStateBase> state;
        StateSubscription           subscription;
    };
    std::vector<Binding> _bindings;

    std::vector<LayoutAppliedCallback> _layoutAppliedListeners;

    friend struct HostState;
};
}
