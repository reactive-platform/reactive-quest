#pragma once

#include <memory>
#include <optional>

#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Vector2.hpp>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/components/ScrollContext.hpp"
#include "reactive/layout/Props.hpp"

namespace UnityEngine::EventSystems { class PointerEventData; }

namespace reactive::components {
enum class ScrollOrientation {
    Horizontal = 0,
    Vertical   = 1,
};

struct ScrollAreaProps {
    REACTIVE_ITEM_PROPS;
    std::shared_ptr<ScrollContext>   scrollContext;
    IReactiveComponent*              scrollContent{nullptr};
    std::optional<ScrollOrientation> scrollOrientation;
    std::optional<float>             lineSize;
    std::optional<float>             joystickScrollSpeed;
};

class ScrollArea : public ReactiveComponent {
public:
    using Props = ScrollAreaProps;

    explicit ScrollArea(Props const& props);

protected:
    UnityEngine::GameObject* Construct() override;
    void OnInitialize() override;
    void OnUpdate() override;
    void OnRectDimensionsChanged() override;

private:
    std::shared_ptr<ScrollContext> _scrollContext;
    IReactiveComponent*            _scrollContent;
    ScrollOrientation              _scrollOrientation;
    float                          _lineSize;
    float                          _joystickScrollSpeed;

    UnityEngine::RectTransform* _viewport{nullptr};
    UnityEngine::RectTransform* _contentTransform{nullptr};

    float _prevContentSize{0.f};
    float _destinationPos{0.f};
    bool  _posSet{false};

    float                Translate(UnityEngine::Vector2 vector) const;
    UnityEngine::Vector2 Translate(float value) const;
    float                SizeOf(UnityEngine::RectTransform* rect) const;

    void ReloadContent();
    void SetDestinationPos(float pos, bool immediately);
    void UpdateContentPos(bool immediately);
    void RefreshMeasurements();
    void RefreshContentSizeIfNeeded();

    void HandlePointerScroll(UnityEngine::EventSystems::PointerEventData* eventData);
};
}
