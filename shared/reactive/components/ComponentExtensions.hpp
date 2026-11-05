#pragma once

#include <functional>
#include <memory>

#include <UnityEngine/CanvasGroup.hpp>
#include <UnityEngine/GameObject.hpp>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/state/State.hpp"

namespace reactive {
struct PointerEventCallbacks {
    std::function<void()> onEnter;
    std::function<void()> onLeave;
    std::function<void()> onDown;
    std::function<void()> onUp;
    std::function<void()> onDrag;
    std::function<void()> onDragBegin;
    std::function<void()> onDragEnd;
    std::function<void()> onScroll;
};

namespace detail {
    void AttachPointerEvents(UnityEngine::GameObject* go, PointerEventCallbacks const& callbacks, std::weak_ptr<void> lifetime);
    void AttachRectMask(UnityEngine::GameObject* go, bool filterRaycasts);
}

template <class Self>
Self* ReactiveComponent::WithPointerEvents(this Self& self, PointerEventCallbacks const& callbacks) {
    detail::AttachPointerEvents(self.Content(), callbacks, self.LifetimeToken());
    return &self;
}

template <class Self>
Self* ReactiveComponent::AsRectMask(this Self& self, bool filterRaycasts) {
    detail::AttachRectMask(self.Content(), filterRaycasts);
    return &self;
}

template <class Self, StateType S>
Self* ReactiveComponent::WithAlpha(this Self& self, std::shared_ptr<S> const& alpha, bool invert) {
    UnityEngine::CanvasGroup* group = nullptr;
    self.WithNativeComponent(group);
    self.On(alpha, [group, invert](Self&, float y) { group->set_alpha(invert ? 1.f - y : y); });
    return &self;
}
}
