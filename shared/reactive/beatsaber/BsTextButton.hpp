#pragma once

#include <string>
#include <string_view>

#include "reactive/beatsaber/BsButton.hpp"
#include "reactive/beatsaber/Label.hpp"
#include "reactive/components/IComponentHolder.hpp"

namespace reactive::beatsaber {
struct BsTextButtonProps {
    REACTIVE_ITEM_PROPS;
    REACTIVE_BS_BUTTON_PROPS;
    REACTIVE_PROP(std::string, text, sText);
    REACTIVE_PROP(float, fontSize, sFontSize);
    REACTIVE_PROP(bool, richText, sRichText);
    REACTIVE_PROP(TMPro::FontStyles, fontStyle, sFontStyle);
};

// A Beat Saber styled button with a label.
class BsTextButton : public ReactiveComponent, public IComponentHolder<Label> {
public:
    using Props = BsTextButtonProps;

    explicit BsTextButton(Props const& props);

    void SetText(std::string_view text) { _label->SetText(text); }
    std::string const& GetText() const { return _label->GetText(); }

    void SetShowUnderline(bool v) { _button->SetShowUnderline(v); }
    void SetSkew(float v) { _button->SetSkew(v); }
    void SetInteractable(bool v) { _button->SetInteractable(v); }
    void SetColors(BsButtonColors colors) { _button->SetColors(std::move(colors)); }

    Label* GetHeldComponent(IComponentHolder<Label>::HolderTag = {}) override { return _label; }

protected:
    UnityEngine::GameObject* Construct() override { return _button->Use(nullptr); }

private:
    Label*    _label;
    BsButton* _button;
};
}
