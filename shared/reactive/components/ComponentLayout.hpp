#pragma once

#include "reactive/layout/Layout.hpp"
#include "reactive/components/IComponentHolder.hpp"

namespace reactive {
// The outer object drives layout while the inner T shares its GameObject.
// T is allocated in the constructor because adapters like Background::SetColor reach it before Construct.
template <class T>
class ComponentLayout : public Layout, public IComponentHolder<T> {
public:
    ComponentLayout() : _component(new T()) {}
    template <HasLayoutProps P>
    explicit ComponentLayout(P const& props) : Layout(props, 0), _component(new T()) {}

    T* GetHeldComponent(typename IComponentHolder<T>::HolderTag = {}) override { return _component; }
    T* Component() const { return _component; }

protected:
    UnityEngine::GameObject* Construct() override {
        return _component->Use(nullptr);
    }

private:
    T* _component;
};
}
