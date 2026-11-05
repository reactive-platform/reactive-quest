#include "reactive/components/PointerEventsHandler.hpp"

#include <UnityEngine/EventSystems/ExecuteEvents.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Transform.hpp>

DEFINE_TYPE(Reactive, PointerEventsHandler);

using PointerEventData = UnityEngine::EventSystems::PointerEventData;

namespace {
    void Notify(std::vector<Reactive::PointerEventsHandler::PointerCallback> const& listeners,
                Reactive::PointerEventsHandler* self, PointerEventData* eventData) {
        for (auto& cb : listeners) cb(self, eventData);
    }
}

void Reactive::PointerEventsHandler::ctor() {
    INVOKE_CTOR();
}

void Reactive::PointerEventsHandler::OnPointerDown(PointerEventData* eventData) {
    IsPressed = true;
    EventData = eventData;
    Notify(_listeners.down, this, eventData);
}

void Reactive::PointerEventsHandler::OnPointerUp(PointerEventData* eventData) {
    IsPressed = false;
    EventData = eventData;
    Notify(_listeners.up, this, eventData);
}

void Reactive::PointerEventsHandler::OnPointerEnter(PointerEventData* eventData) {
    IsHovered = true;
    EventData = eventData;
    Notify(_listeners.enter, this, eventData);
}

void Reactive::PointerEventsHandler::OnPointerExit(PointerEventData* eventData) {
    IsHovered = false;
    EventData = eventData;
    Notify(_listeners.exit, this, eventData);
}

void Reactive::PointerEventsHandler::OnDrag(PointerEventData* eventData) {
    EventData = eventData;
    Notify(_listeners.drag, this, eventData);
}

void Reactive::PointerEventsHandler::OnBeginDrag(PointerEventData* eventData) {
    IsDragging = true;
    EventData  = eventData;
    Notify(_listeners.beginDrag, this, eventData);
}

void Reactive::PointerEventsHandler::OnEndDrag(PointerEventData* eventData) {
    IsDragging = false;
    EventData  = eventData;
    Notify(_listeners.endDrag, this, eventData);
}

void Reactive::PointerEventsHandler::OnScroll(PointerEventData* eventData) {
    EventData = eventData;

    if (!_listeners.scroll.empty()) {
        Notify(_listeners.scroll, this, eventData);
        return;
    }

    // Unity sends scroll only to the first IScrollHandler in the hierarchy, so without a subscriber it goes further up.
    if (auto parent = get_transform()->get_parent()) {
        namespace Events = UnityEngine::EventSystems;
        Events::ExecuteEvents::ExecuteHierarchy<Events::IScrollHandler*>(parent->get_gameObject(), eventData, Events::ExecuteEvents::get_scrollHandler());
    }
}

// Unity doesn't send pointer events to disabled components, so an object disabled while hovered would stay hovered.
void Reactive::PointerEventsHandler::OnDisable() {
    if (!EventData || !IsFocused()) return;

    if (IsDragging) {
        IsDragging = false;
        Notify(_listeners.endDrag, this, EventData);
    }
    if (IsPressed) {
        IsPressed = false;
        Notify(_listeners.up, this, EventData);
    }
    if (IsHovered) {
        IsHovered = false;
        Notify(_listeners.exit, this, EventData);
    }
}

void Reactive::PointerEventsHandler::OnDestroy() {
    _listeners = {};
}
