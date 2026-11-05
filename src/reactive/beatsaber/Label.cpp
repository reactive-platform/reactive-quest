#include "reactive/beatsaber/Label.hpp"

#include <algorithm>

#include <System/Collections/IEnumerator.hpp>
#include <UnityEngine/Events/UnityAction.hpp>
#include <UnityEngine/Time.hpp>
#include <UnityEngine/WaitForEndOfFrame.hpp>

#include "custom-types/shared/delegate.hpp"
#include "reactive/beatsaber/GameResources.hpp"

namespace reactive::beatsaber {
void Label::Construct(UnityEngine::RectTransform* rect) {
    _text = rect->get_gameObject()->AddComponent<HMUI::CurvedTextMeshPro*>();

    // Quest has no prefab with the font baked in, so it's assigned here too.
    if (auto* font = MainTextFont()) _text->set_font(font);
    if (auto* material = UIFontMaterial()) _text->set_fontSharedMaterial(material);

    // Last, because the callback needs the host, which doesn't exist yet during Construct.
    _text->RegisterDirtyLayoutCallback(custom_types::MakeDelegate<UnityEngine::Events::UnityAction*>(
        static_cast<std::function<void()>>([this] { RequestLeafRecalculationOnDirty(); })));
}

void Label::OnInitialize() {
    SetFontSize(_initArgs.fontSize.value_or(4.f));
    SetAlignment(_initArgs.alignment.value_or(TMPro::TextAlignmentOptions::Center));
    SetEnableWrapping(_initArgs.enableWrapping.value_or(false));
    if (_initArgs.richText)         SetRichText(*_initArgs.richText);
    if (_initArgs.fontStyle)        SetFontStyle(*_initArgs.fontStyle);
    if (_initArgs.overflow)         SetOverflow(*_initArgs.overflow);
    if (_initArgs.fontSizeMin)      SetFontSizeMin(*_initArgs.fontSizeMin);
    if (_initArgs.fontSizeMax)      SetFontSizeMax(*_initArgs.fontSizeMax);
    if (_initArgs.enableAutoSizing) SetEnableAutoSizing(*_initArgs.enableAutoSizing);
    if (_initArgs.color)            SetColor(*_initArgs.color);
    if (!_initArgs.text.empty())    SetText(_initArgs.text);
}

void Label::SetText(std::string_view v) {
    _initArgs.text = std::string(v);
    if (_text) _text->set_text(StringW(_initArgs.text));
}

void Label::SetRichText(bool v) {
    _initArgs.richText = v;
    if (_text) _text->set_richText(v);
}

void Label::SetFontSize(float v) {
    _initArgs.fontSize = v;
    if (_text) _text->set_fontSize(v);
}

void Label::SetFontSizeMin(float v) {
    _initArgs.fontSizeMin = v;
    if (_text) _text->set_fontSizeMin(v);
}

void Label::SetFontSizeMax(float v) {
    _initArgs.fontSizeMax = v;
    if (_text) _text->set_fontSizeMax(v);
}

void Label::SetEnableAutoSizing(bool v) {
    _initArgs.enableAutoSizing = v;
    if (_text) _text->set_enableAutoSizing(v);
}

void Label::SetFontStyle(TMPro::FontStyles v) {
    _initArgs.fontStyle = v;
    if (_text) _text->set_fontStyle(v);
}

void Label::SetEnableWrapping(bool v) {
    _initArgs.enableWrapping = v;
    if (_text) _text->set_enableWordWrapping(v);
}

void Label::SetOverflow(TMPro::TextOverflowModes v) {
    _initArgs.overflow = v;
    if (_text) _text->set_overflowMode(v);
}

void Label::SetAlignment(TMPro::TextAlignmentOptions a) {
    _initArgs.alignment = a;
    if (_text) _text->set_alignment(a);
}

void Label::SetColor(UnityEngine::Color c) {
    _initArgs.color = c;
    if (_text) _text->set_color(c);
}

void Label::SetSkew(float v) {
    auto current = _initArgs.fontStyle.value_or(TMPro::FontStyles::Normal);
    auto bit     = TMPro::FontStyles::Italic;
    auto updated = (v > 0.f)
        ? static_cast<TMPro::FontStyles>(static_cast<int>(current) |  static_cast<int>(bit))
        : static_cast<TMPro::FontStyles>(static_cast<int>(current) & ~static_cast<int>(bit));
    SetFontStyle(updated);
}

void Label::OnStart() {
    RequestLeafRecalculation();
}

UnityEngine::Vector2 Label::Measure(float width, MeasureMode widthMode,
                                     float height, MeasureMode heightMode) {
    if (!_text) return {0, 0};
    _lastPreferredSize = _text->GetPreferredValues();
    float w = _lastPreferredSize.x;
    float h = _lastPreferredSize.y;
    if (widthMode == MeasureMode::Exactly) w = width;
    else if (widthMode == MeasureMode::AtMost) w = std::min(w, width);
    if (heightMode == MeasureMode::Exactly) h = height;
    else if (heightMode == MeasureMode::AtMost) h = std::min(h, height);
    return {w, h};
}

void Label::RequestLeafRecalculationOnDirty() {
    const float time = UnityEngine::Time::get_time();
    if (time == _lastRequestTime) return;
    _lastRequestTime = time;

    if (UnityEngine::Vector2::op_Equality(_text->GetPreferredValues(), _lastPreferredSize)) {
        // There's no signal for when the text has actually been recalculated, so this waits a frame like PC.
        StartCoroutine(RequestLeafRecalculationNextFrame());
    } else {
        RequestLeafRecalculation();
    }
}

custom_types::Helpers::Coroutine Label::RequestLeafRecalculationNextFrame() {
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());
    RequestLeafRecalculation();
}

void Label::RequestLeafRecalculation() {
    _lastPreferredSize = _text->GetPreferredValues();
    NotifyLayoutDataUpdated();
}
}
