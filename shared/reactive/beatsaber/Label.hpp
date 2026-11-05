#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <HMUI/CurvedTextMeshPro.hpp>
#include <TMPro/FontStyles.hpp>
#include <TMPro/TextAlignmentOptions.hpp>
#include <TMPro/TextOverflowModes.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/Vector2.hpp>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/layout/Interfaces.hpp"
#include "reactive/layout/Props.hpp"

namespace reactive::beatsaber {
struct LabelProps {
    REACTIVE_ITEM_PROPS;
    REACTIVE_PROP(std::string, text, sText);
    REACTIVE_PROP(bool, richText, sRichText);
    REACTIVE_PROP(float, fontSize, sFontSize);
    REACTIVE_PROP(float, fontSizeMin, sFontSizeMin);
    REACTIVE_PROP(float, fontSizeMax, sFontSizeMax);
    REACTIVE_PROP(bool, enableAutoSizing, sEnableAutoSizing);
    REACTIVE_PROP(TMPro::FontStyles, fontStyle, sFontStyle);
    REACTIVE_PROP(bool, enableWrapping, sEnableWrapping);
    REACTIVE_PROP(TMPro::TextOverflowModes, overflow, sOverflow);
    REACTIVE_PROP(TMPro::TextAlignmentOptions, alignment, sAlignment);
    REACTIVE_PROP(UnityEngine::Color, color, sColor);
    REACTIVE_PROP(float, skew, sSkew);
};

class Label : public ReactiveComponent, public ILeafLayoutItem {
public:
    using Props = LabelProps;

    Label() = default;
    explicit Label(Props const& props) : ReactiveComponent(props) {
        BindProp(props.text, props.sText, [this](std::string const& v) { SetText(v); });
        BindProp(props.richText, props.sRichText, [this](bool v) { SetRichText(v); });
        BindProp(props.fontSize, props.sFontSize, [this](float v) { SetFontSize(v); });
        BindProp(props.fontSizeMin, props.sFontSizeMin, [this](float v) { SetFontSizeMin(v); });
        BindProp(props.fontSizeMax, props.sFontSizeMax, [this](float v) { SetFontSizeMax(v); });
        BindProp(props.enableAutoSizing, props.sEnableAutoSizing, [this](bool v) { SetEnableAutoSizing(v); });
        BindProp(props.fontStyle, props.sFontStyle, [this](TMPro::FontStyles v) { SetFontStyle(v); });
        BindProp(props.enableWrapping, props.sEnableWrapping, [this](bool v) { SetEnableWrapping(v); });
        BindProp(props.overflow, props.sOverflow, [this](TMPro::TextOverflowModes v) { SetOverflow(v); });
        BindProp(props.alignment, props.sAlignment, [this](TMPro::TextAlignmentOptions v) { SetAlignment(v); });
        BindProp(props.color, props.sColor, [this](UnityEngine::Color v) { SetColor(v); });
        BindProp(props.skew, props.sSkew, [this](float v) { SetSkew(v); });
    }

    // Setters are safe before the text exists: the value is stashed and applied in OnInitialize.
    void SetText(std::string_view v);
    void SetRichText(bool v);
    void SetFontSize(float v);
    void SetFontSizeMin(float v);
    void SetFontSizeMax(float v);
    void SetEnableAutoSizing(bool v);
    void SetFontStyle(TMPro::FontStyles v);
    void SetEnableWrapping(bool v);
    void SetOverflow(TMPro::TextOverflowModes v);
    void SetAlignment(TMPro::TextAlignmentOptions a);
    void SetColor(UnityEngine::Color c);
    void SetSkew(float v);

    std::string const&       GetText() const { return _initArgs.text; }
    UnityEngine::Color       GetColor() const { return _initArgs.color.value_or(UnityEngine::Color{1, 1, 1, 1}); }
    HMUI::CurvedTextMeshPro* WrappedText() const { return _text; }

    UnityEngine::Vector2 Measure(float width, MeasureMode widthMode,
                                  float height, MeasureMode heightMode) override;

protected:
    void Construct(UnityEngine::RectTransform* rect) override;
    void OnInitialize() override;
    void OnStart() override;

private:
    struct Args {
        std::string                                  text;
        std::optional<bool>                          richText;
        std::optional<float>                         fontSize;
        std::optional<float>                         fontSizeMin;
        std::optional<float>                         fontSizeMax;
        std::optional<bool>                          enableAutoSizing;
        std::optional<TMPro::FontStyles>             fontStyle;
        std::optional<bool>                          enableWrapping;
        std::optional<TMPro::TextOverflowModes>      overflow;
        std::optional<TMPro::TextAlignmentOptions>   alignment;
        std::optional<UnityEngine::Color>            color;
    };

    HMUI::CurvedTextMeshPro* _text{nullptr};
    Args                     _initArgs{};
    UnityEngine::Vector2     _lastPreferredSize{};
    float                    _lastRequestTime{-1.f};

    void RequestLeafRecalculation();
    void RequestLeafRecalculationOnDirty();
    custom_types::Helpers::Coroutine RequestLeafRecalculationNextFrame();
};
}
