#include "reactive/components/ComponentExtensions.hpp"

#include <UnityEngine/EventSystems/PointerEventData.hpp>
#include <UnityEngine/UI/RectMask2D.hpp>

#include "reactive/beatsaber/RaycastMask2D.hpp"
#include "reactive/components/PointerEventsHandler.hpp"

namespace reactive::detail {
void AttachPointerEvents(UnityEngine::GameObject* go, PointerEventCallbacks const& callbacks, std::weak_ptr<void> lifetime) {
    auto* handler = go->GetComponent<Reactive::PointerEventsHandler*>();
    if (!handler) handler = go->AddComponent<Reactive::PointerEventsHandler*>();

    // A callback usually captures its component, which can already be gone when the handler is disabled while destroying.
    auto wrap = [&lifetime](std::function<void()> const& fn) {
        return [fn, lifetime](Reactive::PointerEventsHandler*, UnityEngine::EventSystems::PointerEventData*) {
            if (!lifetime.expired()) fn();
        };
    };

    if (callbacks.onEnter)     handler->AddPointerEnterListener(wrap(callbacks.onEnter));
    if (callbacks.onLeave)     handler->AddPointerExitListener(wrap(callbacks.onLeave));
    if (callbacks.onDown)      handler->AddPointerDownListener(wrap(callbacks.onDown));
    if (callbacks.onUp)        handler->AddPointerUpListener(wrap(callbacks.onUp));
    if (callbacks.onDrag)      handler->AddDragListener(wrap(callbacks.onDrag));
    if (callbacks.onDragBegin) handler->AddBeginDragListener(wrap(callbacks.onDragBegin));
    if (callbacks.onDragEnd)   handler->AddEndDragListener(wrap(callbacks.onDragEnd));
    if (callbacks.onScroll)    handler->AddScrollListener(wrap(callbacks.onScroll));
}

void AttachRectMask(UnityEngine::GameObject* go, bool filterRaycasts) {
    if (!go->GetComponent<UnityEngine::UI::RectMask2D*>()) {
        go->AddComponent<UnityEngine::UI::RectMask2D*>();
    }
    if (filterRaycasts && !go->GetComponent<Reactive::BeatSaber::RaycastMask2D*>()) {
        go->AddComponent<Reactive::BeatSaber::RaycastMask2D*>();
    }
}
}
