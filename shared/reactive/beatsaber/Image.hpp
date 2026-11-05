#pragma once

#include <optional>

#include <HMUI/ImageView.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/UI/Image.hpp>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/beatsaber/BeatSaberResources.hpp"
#include "reactive/beatsaber/CompositeColorSet.hpp"
#include "reactive/beatsaber/FixedImageView.hpp"
#include "reactive/beatsaber/GameResources.hpp"
#include "reactive/layout/Props.hpp"

#define REACTIVE_IMAGE_PROPS                                                              \
    REACTIVE_PROP(UnityEngine::Sprite*, sprite, sSprite);                                 \
    REACTIVE_PROP(UnityEngine::Color, color, sColor);                                     \
    REACTIVE_PROP(UnityEngine::Material*, material, sMaterial);                           \
    REACTIVE_PROP(bool, preserveAspect, sPreserveAspect);                                 \
    REACTIVE_PROP(UnityEngine::UI::Image::Type, imageType, sImageType);                   \
    REACTIVE_PROP(float, pixelsPerUnit, sPixelsPerUnit);                                  \
    REACTIVE_PROP(bool, raycastTarget, sRaycastTarget);                                   \
    REACTIVE_PROP(bool, useGradient, sUseGradient);                                       \
    REACTIVE_PROP(UnityEngine::Color, gradientColor0, sGradientColor0);                   \
    REACTIVE_PROP(UnityEngine::Color, gradientColor1, sGradientColor1);                   \
    REACTIVE_PROP(HMUI::ImageView::GradientDirection, gradientDirection, sGradientDirection); \
    REACTIVE_PROP(float, skew, sSkew);                                                    \
    REACTIVE_PROP(::reactive::beatsaber::CompositeColors, colors, sColors)

namespace reactive::beatsaber {
struct BackgroundArgs {
    std::optional<UnityEngine::Color>                 color;
    UnityEngine::Sprite*                              sprite{nullptr};
    UnityEngine::Material*                            material{nullptr};
    std::optional<UnityEngine::UI::Image::Type>       imageType;
    std::optional<bool>                               raycastTarget;
    std::optional<float>                              pixelsPerUnit;
    std::optional<float>                              skew;
    std::optional<HMUI::ImageView::GradientDirection> gradientDirection;
    UnityEngine::Color                                gradientColor0{0, 0, 0, 0};
    UnityEngine::Color                                gradientColor1{0, 0, 0, 0};
};

struct ImageProps {
    REACTIVE_ITEM_PROPS;
    REACTIVE_IMAGE_PROPS;
};

class Image : public ReactiveComponent {
public:
    using Props = ImageProps;

    Image() = default;
    explicit Image(Props const& props) : ReactiveComponent(props) { ApplyImageProps(props); }

    // Setters are safe before the image exists: the value is stashed and applied in Construct.
    void SetSprite(UnityEngine::Sprite* s);
    void SetColor(UnityEngine::Color c);
    void SetMaterial(UnityEngine::Material* m);
    void SetPreserveAspect(bool v);
    void SetImageType(UnityEngine::UI::Image::Type t);
    void SetPixelsPerUnit(float v);
    void SetRaycastTarget(bool v);

    void SetUseGradient(bool v);
    void SetGradientColor0(UnityEngine::Color c);
    void SetGradientColor1(UnityEngine::Color c);
    void SetGradientDirection(HMUI::ImageView::GradientDirection d);
    void SetSkew(float v);
    void SetColors(CompositeColors const& colors);

    void ApplyBackground(BackgroundArgs const& args);
    void ApplyBlurBackground(float pixelsPerUnit, std::optional<UnityEngine::Color> color);

    UnityEngine::Sprite* GetSprite() const { return _stash.sprite; }
    UnityEngine::Color   GetColor() const { return _stash.color; }
    float                GetSkew() const { return _stash.skew.value_or(0.f); }

    Reactive::BeatSaber::FixedImageView* WrappedImage() const { return _image; }

    template <class Self>
    Self* AsBackground(this Self& self, BackgroundArgs const& args = {}) {
        self.ApplyBackground(args);
        return &self;
    }

    template <class Self>
    Self* AsBlurBackground(this Self& self, float pixelsPerUnit = 12.f,
                           std::optional<UnityEngine::Color> color = std::nullopt) {
        self.ApplyBlurBackground(pixelsPerUnit, color);
        return &self;
    }

    template <class P>
    void ApplyImageProps(P const& p) {
        BindProp(p.sprite, p.sSprite, [this](UnityEngine::Sprite* v) { SetSprite(v); });
        BindProp(p.color, p.sColor, [this](UnityEngine::Color v) { SetColor(v); });
        BindProp(p.material, p.sMaterial, [this](UnityEngine::Material* v) { SetMaterial(v); });
        BindProp(p.preserveAspect, p.sPreserveAspect, [this](bool v) { SetPreserveAspect(v); });
        BindProp(p.imageType, p.sImageType, [this](UnityEngine::UI::Image::Type v) { SetImageType(v); });
        BindProp(p.pixelsPerUnit, p.sPixelsPerUnit, [this](float v) { SetPixelsPerUnit(v); });
        BindProp(p.raycastTarget, p.sRaycastTarget, [this](bool v) { SetRaycastTarget(v); });
        BindProp(p.useGradient, p.sUseGradient, [this](bool v) { SetUseGradient(v); });
        BindProp(p.gradientColor0, p.sGradientColor0, [this](UnityEngine::Color v) { SetGradientColor0(v); });
        BindProp(p.gradientColor1, p.sGradientColor1, [this](UnityEngine::Color v) { SetGradientColor1(v); });
        BindProp(p.gradientDirection, p.sGradientDirection,
                 [this](HMUI::ImageView::GradientDirection v) { SetGradientDirection(v); });
        BindProp(p.skew, p.sSkew, [this](float v) { SetSkew(v); });
        BindProp(p.colors, p.sColors, [this](CompositeColors const& v) { SetColors(v); });
    }

protected:
    template <HasItemProps P>
    Image(P const& props, int) : ReactiveComponent(props) {
        ApplyImageProps(props);
    }

    void Construct(UnityEngine::RectTransform* rect) override;

private:
    struct Stash {
        UnityEngine::Color                                color{1, 1, 1, 1};
        UnityEngine::Sprite*                              sprite{nullptr};
        UnityEngine::Material*                            material{nullptr};
        std::optional<UnityEngine::UI::Image::Type>       imageType;
        std::optional<bool>                               preserveAspect;
        std::optional<float>                              pixelsPerUnit;
        std::optional<bool>                               raycastTarget;
        std::optional<UnityEngine::Color>                 gradientColor0;
        std::optional<UnityEngine::Color>                 gradientColor1;
        std::optional<bool>                               useGradient;
        std::optional<HMUI::ImageView::GradientDirection> gradientDirection;
        std::optional<float>                              skew;
    };

    Reactive::BeatSaber::FixedImageView* _image{nullptr};
    Stash                                 _stash{};
};
}
