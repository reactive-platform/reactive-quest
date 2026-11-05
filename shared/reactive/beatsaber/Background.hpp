#pragma once

#include <optional>

#include <HMUI/ImageView.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/UI/Image.hpp>

#include "reactive/beatsaber/BeatSaberResources.hpp"
#include "reactive/beatsaber/GameResources.hpp"
#include "reactive/beatsaber/Image.hpp"
#include "reactive/components/ComponentLayout.hpp"

namespace reactive::beatsaber {
struct BackgroundProps {
    REACTIVE_LAYOUT_PROPS;
    REACTIVE_IMAGE_PROPS;
};

class Background : public ComponentLayout<Image> {
public:
    using Props = BackgroundProps;

    Background() = default;
    explicit Background(Props const& props) : ComponentLayout<Image>(props) {
        Component()->ApplyImageProps(props);
    }

    void SetSprite(UnityEngine::Sprite* s)         { Component()->SetSprite(s); }
    void SetColor(UnityEngine::Color c)            { Component()->SetColor(c); }
    void SetMaterial(UnityEngine::Material* m)     { Component()->SetMaterial(m); }
    void SetPreserveAspect(bool v)                 { Component()->SetPreserveAspect(v); }
    void SetImageType(UnityEngine::UI::Image::Type t) { Component()->SetImageType(t); }
    void SetPixelsPerUnit(float v)                 { Component()->SetPixelsPerUnit(v); }
    void SetRaycastTarget(bool v)                  { Component()->SetRaycastTarget(v); }
    void SetUseGradient(bool v)                    { Component()->SetUseGradient(v); }
    void SetGradientColor0(UnityEngine::Color c)   { Component()->SetGradientColor0(c); }
    void SetGradientColor1(UnityEngine::Color c)   { Component()->SetGradientColor1(c); }
    void SetGradientDirection(HMUI::ImageView::GradientDirection d) { Component()->SetGradientDirection(d); }
    void SetSkew(float v)                          { Component()->SetSkew(v); }
    void SetColors(CompositeColors const& colors)  { Component()->SetColors(colors); }

    UnityEngine::Color GetColor() const { return Component()->GetColor(); }
    Image*             WrappedImage() const { return Component(); }

    template <class Self>
    Self* AsBackground(this Self& self, BackgroundArgs const& args = {}) {
        self.WrappedImage()->ApplyBackground(args);
        return &self;
    }

    template <class Self>
    Self* AsBlurBackground(this Self& self, float pixelsPerUnit = 12.f,
                           std::optional<UnityEngine::Color> color = std::nullopt) {
        self.WrappedImage()->ApplyBlurBackground(pixelsPerUnit, color);
        return &self;
    }
};
}
