#include "reactive/beatsaber/BsTextButton.hpp"

#include <TMPro/FontStyles.hpp>

namespace reactive::beatsaber {
BsTextButton::BsTextButton(Props const& props) : ReactiveComponent(props), _label(new Label()) {
    _label->SetFontStyle(TMPro::FontStyles::UpperCase);

    BindProp(props.text, props.sText, [this](std::string const& v) { _label->SetText(v); });
    BindProp(props.fontSize, props.sFontSize, [this](float v) { _label->SetFontSize(v); });
    BindProp(props.richText, props.sRichText, [this](bool v) { _label->SetRichText(v); });
    BindProp(props.fontStyle, props.sFontStyle, [this](TMPro::FontStyles v) { _label->SetFontStyle(v); });

    _button = new BsButton({
        .onClick          = props.onClick,
        .interactable     = props.interactable,
        .sInteractable    = props.sInteractable,
        .showUnderline    = props.showUnderline,
        .skew             = props.skew,
        .colors           = props.colors,
        .constructContent = [label = _label](std::shared_ptr<IState<UnityEngine::Color>> color, std::shared_ptr<IState<float>> skew) -> IReactiveComponent* {
            label->On(color, [](Label& x, UnityEngine::Color const& c) { x.SetColor(c); });
            label->On(skew, [](Label& x, float v) { x.SetSkew(v); });
            return label;
        },
    });
}
}
