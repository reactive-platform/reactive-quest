#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "reactive/ReactiveComponent.hpp"

namespace reactive {
template <class T>
class ReactivePool {
    static_assert(std::is_base_of_v<ReactiveComponent, T>, "ReactivePool<T> requires T : ReactiveComponent");

public:
    std::function<T*()> Construct;
    bool                DetachOnDespawn{true};

    T* Spawn(bool enable = true) {
        // A reserved item can be destroyed along with whatever it was parented to.
        while (!_reserved.empty()) {
            auto item = _reserved.back();
            _reserved.pop_back();
            if (!item.lifetime.expired()) return Activate(item, enable);
        }
        T* comp = Construct ? Construct() : new T();
        return Activate({comp, comp->LifetimeToken()}, enable);
    }

    void Despawn(T* comp) {
        auto it = std::find_if(_spawned.begin(), _spawned.end(), [comp](Item const& x) { return x.comp == comp; });
        if (it == _spawned.end()) return;
        auto item = *it;
        _spawned.erase(it);
        Reserve(item);
    }

    void DespawnAll() {
        auto spawned = std::move(_spawned);
        _spawned.clear();
        for (auto const& item : spawned) Reserve(item);
    }

private:
    struct Item {
        T*                  comp;
        std::weak_ptr<void> lifetime;
    };

    std::vector<Item> _spawned;
    std::vector<Item> _reserved;

    T* Activate(Item const& item, bool enable) {
        _spawned.push_back(item);
        if (enable) item.comp->SetEnabled(true);
        return item.comp;
    }

    void Reserve(Item const& item) {
        if (item.lifetime.expired()) return;
        item.comp->SetEnabled(false);
        if (DetachOnDespawn) item.comp->Use(nullptr);
        _reserved.push_back(item);
    }
};
}
