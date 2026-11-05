#include "reactive/components/ScrollArea.hpp"

#include <UnityEngine/EventSystems/PointerEventData.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Time.hpp>
#include <UnityEngine/UI/Image.hpp>
#include <UnityEngine/UI/RectMask2D.hpp>
#include <UnityEngine/Vector3.hpp>

#include "reactive/components/PointerEventsHandler.hpp"

namespace reactive::components {
ScrollArea::ScrollArea(Props const& props)
    : ReactiveComponent(props),
      _scrollContext(props.scrollContext),
      _scrollContent(props.scrollContent),
      _scrollOrientation(props.scrollOrientation.value_or(ScrollOrientation::Vertical)),
      _lineSize(props.lineSize.value_or(10.f)),
      // The game's lists: HMUI.ScrollView._joystickScrollSpeed
      _joystickScrollSpeed(props.joystickScrollSpeed.value_or(60.f)) {}

UnityEngine::GameObject* ScrollArea::Construct() {
    auto* go  = UnityEngine::GameObject::New_ctor();
    _viewport = go->AddComponent<UnityEngine::RectTransform*>();
    _viewport->set_pivot({1.f, 1.f});

    // Invisible, it's only there to receive raycasts.
    go->AddComponent<UnityEngine::UI::Image*>()->set_color({1.f, 1.f, 1.f, 0.f});
    go->AddComponent<UnityEngine::UI::RectMask2D*>();

    go->AddComponent<Reactive::PointerEventsHandler*>()->AddScrollListener(
        [this](Reactive::PointerEventsHandler*, UnityEngine::EventSystems::PointerEventData* eventData) {
            HandlePointerScroll(eventData);
        });
    return go;
}

void ScrollArea::OnInitialize() {
    _scrollContent->Use(_viewport);
    _contentTransform = _scrollContent->ContentTransform();
    ReloadContent();

    BindState<ScrollContext>(_scrollContext, [this](ScrollContext const& context) {
        if (context.HasUpdate(ScrollUpdateType::Intent)) SetDestinationPos(context.ScrollPos(), context.Immediately());
    });
}

void ScrollArea::OnUpdate() {
    const float contentSize = SizeOf(_contentTransform);
    if (_prevContentSize != contentSize) {
        RefreshMeasurements();
        _prevContentSize = contentSize;
        // The destination may be out of the content now.
        _posSet = false;
    }

    UpdateContentPos(false);
}

void ScrollArea::OnRectDimensionsChanged() {
    if (!_contentTransform) return;
    RefreshMeasurements();

    // Resizing the viewport moves content off.
    _posSet = false;
    UpdateContentPos(true);
}

float ScrollArea::Translate(UnityEngine::Vector2 vector) const {
    return _scrollOrientation == ScrollOrientation::Vertical ? vector.y : vector.x;
}

UnityEngine::Vector2 ScrollArea::Translate(float value) const {
    if (_scrollOrientation == ScrollOrientation::Vertical) return {0.f, value};
    return {value, 0.f};
}

float ScrollArea::SizeOf(UnityEngine::RectTransform* rect) const {
    auto r = rect->get_rect();
    return _scrollOrientation == ScrollOrientation::Vertical ? r.m_Height : r.m_Width;
}

void ScrollArea::ReloadContent() {
    auto sizeDelta = _contentTransform->get_sizeDelta();
    if (_scrollOrientation == ScrollOrientation::Vertical) {
        _contentTransform->set_anchorMin({0.f, 1.f});
        _contentTransform->set_anchorMax({1.f, 1.f});
        _contentTransform->set_sizeDelta({0.f, sizeDelta.y});
    } else {
        _contentTransform->set_anchorMin({1.f, 0.f});
        _contentTransform->set_anchorMax({1.f, 1.f});
        _contentTransform->set_sizeDelta({sizeDelta.x, 0.f});
    }
    _contentTransform->set_pivot({1.f, 1.f});

    RefreshMeasurements();
}

void ScrollArea::SetDestinationPos(float pos, bool immediately) {
    if (anim::Approximately(_destinationPos, pos)) return;

    RefreshContentSizeIfNeeded();

    _destinationPos = anim::Clamp(pos, 0.f, _scrollContext->MaxScrollPos());
    _posSet         = false;

    if (immediately) UpdateContentPos(true);
}

void ScrollArea::UpdateContentPos(bool immediately) {
    if (_posSet) return;

    RefreshContentSizeIfNeeded();

    // The content or the viewport can change after the destination was set.
    const float maxPos = _scrollContext->MaxScrollPos();
    if (_destinationPos > maxPos) {
        // Snapping, since animating would show the empty space past the content.
        _destinationPos = maxPos;
        immediately     = true;
    }

    auto source = _contentTransform->get_localPosition();
    UnityEngine::Vector2 sourcePos{source.x, source.y};
    auto destinationPos = Translate(_destinationPos);

    if (!immediately) {
        destinationPos = UnityEngine::Vector2::Lerp(sourcePos, destinationPos, UnityEngine::Time::get_deltaTime() * 4.f);
    }

    const float translated = Translate(destinationPos);

    if (UnityEngine::Vector2::op_Equality(sourcePos, destinationPos)) {
        _posSet = true;
        _scrollContext->ControllerSetScrollPos(translated);
        _scrollContext->ControllerNotifyScrollCompleted();
        return;
    }

    _contentTransform->set_localPosition({destinationPos.x, destinationPos.y, 0.f});
    _scrollContext->ControllerSetScrollPos(translated);
}

void ScrollArea::RefreshMeasurements() {
    _scrollContext->ControllerSetMeasurements(SizeOf(_contentTransform), SizeOf(_viewport), _lineSize);
}

void ScrollArea::RefreshContentSizeIfNeeded() {
    if (SizeOf(_contentTransform) != _scrollContext->ContentSize()) RefreshMeasurements();
}

void ScrollArea::HandlePointerScroll(UnityEngine::EventSystems::PointerEventData* eventData) {
    const float multiplier = _joystickScrollSpeed * UnityEngine::Time::get_deltaTime();
    const float neg        = _scrollOrientation == ScrollOrientation::Vertical ? -1.f : 1.f;

    _scrollContext->ScrollTo(_destinationPos + eventData->get_scrollDelta().y * multiplier * neg, false);
}
}
