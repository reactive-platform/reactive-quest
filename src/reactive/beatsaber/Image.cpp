#include "reactive/beatsaber/Image.hpp"

#include "reactive/beatsaber/GameResources.hpp"

namespace reactive::beatsaber {
void Image::Construct(UnityEngine::RectTransform* rect) {
    _image = rect->get_gameObject()->AddComponent<Reactive::BeatSaber::FixedImageView*>();

    if (auto* material = _stash.material ? _stash.material : UINoGlowMaterial()) _image->set_material(material);

    _image->set_color(_stash.color);
    if (_stash.sprite)         _image->set_sprite(_stash.sprite);
    if (_stash.preserveAspect) _image->set_preserveAspect(*_stash.preserveAspect);
    if (_stash.imageType)      _image->set_type(*_stash.imageType);
    if (_stash.pixelsPerUnit) {
        _image->set_type(UnityEngine::UI::Image::Type::Sliced);
        _image->set_pixelsPerUnitMultiplier(*_stash.pixelsPerUnit);
    }
    if (_stash.raycastTarget)     _image->set_raycastTarget(*_stash.raycastTarget);
    if (_stash.gradientColor0)    _image->set_color0(*_stash.gradientColor0);
    if (_stash.gradientColor1)    _image->set_color1(*_stash.gradientColor1);
    if (_stash.useGradient)       _image->set_gradient(*_stash.useGradient);
    if (_stash.gradientDirection) _image->_gradientDirection = *_stash.gradientDirection;
    if (_stash.skew) {
        _image->_skew = *_stash.skew;
        _image->__Refresh();
    }
}

void Image::SetSprite(UnityEngine::Sprite* s)       { _stash.sprite = s;       if (_image) _image->set_sprite(s); }
void Image::SetColor(UnityEngine::Color c)          { _stash.color = c;        if (_image) _image->set_color(c); }
void Image::SetMaterial(UnityEngine::Material* m)   { _stash.material = m;     if (_image) _image->set_material(m); }
void Image::SetPreserveAspect(bool v)               { _stash.preserveAspect = v; if (_image) _image->set_preserveAspect(v); }
void Image::SetImageType(UnityEngine::UI::Image::Type t) {
    _stash.imageType = t;
    if (_image) _image->set_type(t);
}
void Image::SetPixelsPerUnit(float v) {
    _stash.pixelsPerUnit = v;
    if (_image) {
        _image->set_type(UnityEngine::UI::Image::Type::Sliced);
        _image->set_pixelsPerUnitMultiplier(v);
    }
}
void Image::SetRaycastTarget(bool v)                { _stash.raycastTarget = v; if (_image) _image->set_raycastTarget(v); }

void Image::SetUseGradient(bool v)                  { _stash.useGradient = v;     if (_image) _image->set_gradient(v); }
void Image::SetGradientColor0(UnityEngine::Color c) { _stash.gradientColor0 = c;  if (_image) _image->set_color0(c); }
void Image::SetGradientColor1(UnityEngine::Color c) { _stash.gradientColor1 = c;  if (_image) _image->set_color1(c); }
void Image::SetGradientDirection(HMUI::ImageView::GradientDirection d) {
    _stash.gradientDirection = d;
    if (_image) _image->_gradientDirection = d;
}
void Image::SetSkew(float v) {
    _stash.skew = v;
    if (_image) {
        _image->_skew = v;
        _image->__Refresh();
    }
}

void Image::SetColors(CompositeColors const& colors) {
    SetColor(colors.color);
    SetGradientColor0(colors.gradientColor0);
    SetGradientColor1(colors.gradientColor1);
}

void Image::ApplyBackground(BackgroundArgs const& args) {
    auto* sprite = args.sprite;
    if (!sprite) {
        if (auto* sprites = Sprites()) sprite = sprites->background;
    }
    if (sprite) SetSprite(sprite);

    if (auto* material = args.material ? args.material : UINoGlowMaterial()) SetMaterial(material);

    SetImageType(args.imageType.value_or(UnityEngine::UI::Image::Type::Sliced));
    SetPixelsPerUnit(args.pixelsPerUnit.value_or(10.f));
    SetColor(args.color.value_or(UnityEngine::Color{1, 1, 1, 1}));
    SetSkew(args.skew.value_or(0.f));
    if (args.raycastTarget) SetRaycastTarget(*args.raycastTarget);
    if (args.gradientDirection) {
        SetUseGradient(true);
        SetGradientDirection(*args.gradientDirection);
        SetGradientColor0(args.gradientColor0);
        SetGradientColor1(args.gradientColor1);
    }
}

void Image::ApplyBlurBackground(float pixelsPerUnit, std::optional<UnityEngine::Color> color) {
    if (!GetSprite()) {
        if (auto* sprites = Sprites()) SetSprite(sprites->background);
    }
    if (auto* material = UIFogBackgroundMaterial()) SetMaterial(material);
    if (color) SetColor(*color);
    SetPixelsPerUnit(pixelsPerUnit);
}
}
