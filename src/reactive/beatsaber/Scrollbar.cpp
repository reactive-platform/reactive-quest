#include "reactive/beatsaber/Scrollbar.hpp"

#include <UnityEngine/UI/Image.hpp>
#include <UnityEngine/Vector3.hpp>

#include "reactive/beatsaber/Background.hpp"
#include "reactive/beatsaber/BeatSaberResources.hpp"
#include "reactive/beatsaber/GameResources.hpp"
#include "reactive/beatsaber/Image.hpp"
#include "reactive/components/ComponentExtensions.hpp"
#include "reactive/layout/Layout.hpp"

namespace reactive::beatsaber {
using ::reactive::components::ScrollContext;

namespace {
    constexpr float kPadding = 0.25f;
}

Scrollbar::Scrollbar(Props const& props)
    : ReactiveComponent(props),
      _scrollContext(props.scrollContext),
      _scrollMode(props.scrollMode.value_or(ScrollbarScrollMode::Page)),
      _fixedScrollSize(props.fixedScrollSize.value_or(10.f)),
      _hideIfNothingToScroll(props.hideIfNothingToScroll.value_or(false)) {}

void Scrollbar::Scroll(bool down) {
    switch (_scrollMode) {
        case ScrollbarScrollMode::Page:
            down ? _scrollContext->PageDown() : _scrollContext->PageUp();
            break;
        case ScrollbarScrollMode::Line:
            down ? _scrollContext->LineDown() : _scrollContext->LineUp();
            break;
        case ScrollbarScrollMode::Fixed:
            _scrollContext->ScrollRelative(down ? _fixedScrollSize : -_fixedScrollSize);
            break;
    }
}

void Scrollbar::RefreshHandle() {
    const float areaHeight   = _handleContainerRect->get_rect().m_Height - 2.f * kPadding;
    const float handleHeight = _scrollContext->NormalizedPageHeight() * areaHeight;
    const float handlePos    = (areaHeight - handleHeight) * _scrollContext->NormalizedScrollPos();

    _handleRect->set_sizeDelta({0.f, handleHeight});
    _handleRect->set_localPosition({0.f, -(kPadding + handlePos), 0.f});
}

IReactiveComponent* Scrollbar::CreateButton(bool topButton) {
    auto* sprites = Sprites();
    auto  hovered = Remember(false);
    auto  enabled = _scrollContext->Map([topButton](ScrollContext const& context) {
        return topButton ? context.CanScrollUp() : context.CanScrollDown();
    });

    auto* arrow = new Image({
        .name           = "Button",
        .sprite         = ArrowIcon(),
        .sColor         = RememberDerived(hovered, enabled, [](bool hovered, bool enabled) {
            return enabled ? UnityEngine::Color{1.f, 1.f, 1.f, hovered ? 1.f : 0.5f} : UnityEngine::Color{0.f, 0.f, 0.f, 0.5f};
        }),
        .preserveAspect = true,
    });
    auto* arrowRect = arrow->ContentTransform();
    arrowRect->set_pivot({0.5f, 0.f});
    arrowRect->set_sizeDelta({2.5f, 2.5f});
    arrowRect->set_anchorMin({0.5f, topButton ? 1.f : 0.f});
    arrowRect->set_anchorMax({0.5f, topButton ? 1.f : 0.f});
    arrowRect->set_localEulerAngles({0.f, 0.f, topButton ? 180.f : 0.f});
    arrow->On(hovered, [arrowRect](bool hovered) {
        const float scale = hovered ? 1.2f : 1.f;
        arrowRect->set_localScale({scale, scale, scale});
    });

    // Wrapped in a background to get a wider clickable area.
    auto* area = new Background({
        .name     = "ButtonArea",
        .children = {arrow},
        .sprite   = sprites ? sprites->transparentPixel : nullptr,
    });
    auto* areaRect = area->ContentTransform();
    areaRect->set_anchorMin({0.f, topButton ? 0.5f : 0.f});
    areaRect->set_anchorMax({1.f, topButton ? 1.f : 0.5f});
    areaRect->set_sizeDelta({0.f, 0.f});

    area->WithPointerEvents({
        .onEnter = [hovered] { hovered->SetValue(true); },
        .onLeave = [hovered] { hovered->SetValue(false); },
        .onDown  = [this, topButton] { Scroll(!topButton); },
    });
    return area;
}

UnityEngine::GameObject* Scrollbar::Construct() {
    auto* sprites = Sprites();

    auto* handle = new Image({
        .name      = "Handle",
        .sprite    = VerticalIndicatorIcon(),
        .color     = UnityEngine::Color{1.f, 1.f, 1.f, 0.5f},
        .imageType = UnityEngine::UI::Image::Type::Sliced,
    });
    _handleRect = handle->ContentTransform();
    _handleRect->set_anchorMin({0.f, 1.f});
    _handleRect->set_anchorMax({1.f, 1.f});
    _handleRect->set_pivot({0.5f, 1.f});

    auto* handleContainer = new Background({
        .name          = "HandleContainer",
        .children      = {handle},
        .sprite        = sprites ? sprites->background : nullptr,
        .color         = UnityEngine::Color{0.f, 0.f, 0.f, 0.5f},
        .imageType     = UnityEngine::UI::Image::Type::Sliced,
        .pixelsPerUnit = 20.f,
    });
    _handleContainerRect = handleContainer->ContentTransform();
    _handleContainerRect->set_pivot({0.5f, 1.f});
    _handleContainerRect->set_anchorMin({0.5f, 0.f});
    _handleContainerRect->set_anchorMax({0.5f, 1.f});
    _handleContainerRect->set_offsetMin({-0.7f, 4.f});
    _handleContainerRect->set_offsetMax({0.7f, -4.f});

    // No flex controller, so these keep their anchors. The item props are shared with the Scrollbar as its defaults.
    return (new Layout({
        .flexItem = {.size = {.x = 4.f}},
        .sEnabled = _scrollContext->Map([hide = _hideIfNothingToScroll](ScrollContext const& context) {
            return !hide || context.CanScroll();
        }),
        .withinLayoutIfDisabled = true,
        .children = {handleContainer, CreateButton(true), CreateButton(false)},
    }))->Use(nullptr);
}

void Scrollbar::OnInitialize() {
    BindState<ScrollContext>(_scrollContext, [this](ScrollContext const&) { RefreshHandle(); });
}
}
