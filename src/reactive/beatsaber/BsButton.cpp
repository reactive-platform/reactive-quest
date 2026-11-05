#include "reactive/beatsaber/BsButton.hpp"

#include <UnityEngine/UI/Image.hpp>

#include "reactive/beatsaber/Background.hpp"
#include "reactive/beatsaber/BeatSaberResources.hpp"
#include "reactive/beatsaber/GameResources.hpp"
#include "reactive/beatsaber/Image.hpp"
#include "reactive/components/ComponentExtensions.hpp"
#include "reactive/layout/LayoutExtensions.hpp"

namespace reactive::beatsaber {
BsButton::BsButton(Props const& props)
    : ReactiveComponent(props),
      OnClick(props.onClick),
      _constructContent(props.constructContent),
      _underlineEnabled(Remember(props.showUnderline.value_or(true))),
      _skew(Remember(props.skew.value_or(BeatSaberStyle::Skew))),
      _graphicState(Remember(GraphicState::None)),
      _colors(Remember(props.colors.value_or(BeatSaberStyle::ButtonColors))) {
    BindProp(props.interactable, props.sInteractable, [this](bool v) { SetInteractable(v); });
}

UnityEngine::GameObject* BsButton::Construct() {
    auto* sprites = Sprites();

    auto backgroundColors = RememberDerived(_graphicState, _colors, [](GraphicState state, BsButtonColors const& colors) {
        return colors.backgroundColors.GetColors(state);
    });
    auto underlineColor = RememberDerived(_graphicState, _colors, [](GraphicState state, BsButtonColors const& colors) {
        return colors.underlineColors.GetColor(state);
    });
    auto contentColor = RememberDerived(_graphicState, _colors, [](GraphicState state, BsButtonColors const& colors) {
        return colors.contentColors.GetColor(state);
    });

    auto* background = new Background({
        .flexController = {
            .justifyContent = Justify::SpaceAround,
            .padding        = {.left = 1.f, .right = 1.f},
        },
        .flexItem      = {.size = {.y = 8.f}},
        .sprite        = sprites ? sprites->background : nullptr,
        .pixelsPerUnit = 12.f,
        .useGradient   = true,
        .sSkew         = _skew,
        .sColors       = backgroundColors,
    });

    background->WithPointerEvents({
        .onEnter = [this] { SetHovered(*_graphicState, true); },
        .onLeave = [this] { SetHovered(*_graphicState, false); },
        .onDown  = [this] {
            if (!IsInteractable(_graphicState->Value())) return;
            RaiseButtonClickSignal();
            if (OnClick) OnClick();
        },
    });

    // Parented directly rather than added as a child, which keeps it out of the flex layout.
    auto* underline = new Image({
        .sEnabled      = _underlineEnabled,
        .name          = "Underline",
        .sprite        = sprites ? sprites->backgroundUnderline : nullptr,
        .sColor        = underlineColor,
        .imageType     = UnityEngine::UI::Image::Type::Sliced,
        .pixelsPerUnit = 12.f,
        .sSkew         = _skew,
    });
    underline->WithRectExpand();
    underline->Use(background->ContentTransform());

    if (_constructContent) background->AddChild(_constructContent(contentColor, _skew));

    return background->Use(nullptr);
}
}
