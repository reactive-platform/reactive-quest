#pragma once

#include <algorithm>
#include <cstdint>

#include "reactive/animations/AnimationMath.hpp"
#include "reactive/state/State.hpp"

namespace reactive::components {
enum class ScrollUpdateType : uint8_t {
    None           = 0,
    Intent         = 1 << 0,  // The user wants to scroll.
    Measurements   = 1 << 1,  // The controller changed the content or view size.
    Scroll         = 1 << 2,  // The controller moved the content.
    ScrollFinished = 1 << 3,  // The controller has just finished moving the content.
};

// Links a scroll controller with whatever follows or drives it, such as a scrollbar.
class ScrollContext final : public StateBase<ScrollContext> {
public:
    ScrollContext const& Value() const override { return *this; }

    float ScrollPos() const { return _scrollPos; }
    bool  Immediately() const { return _immediately; }
    float ActualScrollPos() const { return _actualScrollPos; }
    float LineSize() const { return _lineSize; }
    float ContentSize() const { return _contentSize; }
    float ViewSize() const { return _viewSize; }

    // The kinds of updates in the last flush. Kept until the next flush.
    bool HasUpdate(ScrollUpdateType type) const { return (_updateType & static_cast<uint8_t>(type)) != 0; }

    float MaxScrollPos() const { return std::max(0.f, _contentSize - _viewSize); }
    float NormalizedScrollPos() const { return MaxScrollPos() > 0.f ? anim::Clamp01(_actualScrollPos / MaxScrollPos()) : 0.f; }
    float NormalizedPageHeight() const { return _contentSize > 0.f ? anim::Clamp01(_viewSize / _contentSize) : 1.f; }
    bool  CanScrollUp() const { return _scrollPos > 0.f; }
    bool  CanScrollDown() const { return _scrollPos < MaxScrollPos(); }
    bool  CanScroll() const { return _contentSize > _viewSize; }

    void ScrollTo(float pos, bool immediately = false) {
        const float newPos = anim::Clamp(pos, 0.f, MaxScrollPos());
        if (anim::Approximately(newPos, _scrollPos)) return;

        _scrollPos   = newPos;
        _immediately = immediately;
        NotifyValueChanged(ScrollUpdateType::Intent);
    }

    void ScrollRelative(float offset, bool immediately = false) { ScrollTo(_scrollPos + offset, immediately); }
    void PageDown(bool immediately = false) { ScrollRelative(_viewSize, immediately); }
    void PageUp(bool immediately = false) { ScrollRelative(-_viewSize, immediately); }
    void LineDown(bool immediately = false) { ScrollRelative(_lineSize, immediately); }
    void LineUp(bool immediately = false) { ScrollRelative(-_lineSize, immediately); }

    // Only for the component controlling the view.
    void ControllerSetMeasurements(float contentSize, float viewSize, float lineSize) {
        _contentSize = contentSize;
        _viewSize    = viewSize;
        _lineSize    = lineSize;
        if (_scrollPos > MaxScrollPos()) _scrollPos = MaxScrollPos();
        NotifyValueChanged(ScrollUpdateType::Measurements);
    }

    void ControllerSetScrollPos(float pos) {
        if (anim::Approximately(_actualScrollPos, pos)) return;
        _actualScrollPos = pos;
        NotifyValueChanged(ScrollUpdateType::Scroll);
    }

    void ControllerNotifyScrollCompleted() { NotifyValueChanged(ScrollUpdateType::ScrollFinished); }

protected:
    void OnFlush() override {
        _updateType        = _pendingUpdateType;
        _pendingUpdateType = 0;
    }

private:
    float   _scrollPos{0.f};
    bool    _immediately{false};
    float   _actualScrollPos{0.f};
    float   _lineSize{0.f};
    float   _contentSize{0.f};
    float   _viewSize{0.f};
    uint8_t _updateType{0};
    uint8_t _pendingUpdateType{0};

    void NotifyValueChanged(ScrollUpdateType type) {
        _pendingUpdateType |= static_cast<uint8_t>(type);
        this->NotifyValueInvalidated();
    }
};
}
