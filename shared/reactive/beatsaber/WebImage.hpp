#pragma once

#include <functional>
#include <string>

#include <UnityEngine/Sprite.hpp>

#include "reactive/beatsaber/Image.hpp"

namespace reactive::beatsaber {
struct WebImageProps {
    REACTIVE_ITEM_PROPS;
    REACTIVE_IMAGE_PROPS;
    REACTIVE_PROP(std::string, src, sSrc);
    UnityEngine::Sprite* fallbackSprite{nullptr};
};

class WebImage : public Image {
public:
    using Props = WebImageProps;

    WebImage() = default;
    explicit WebImage(Props const& props) : Image(props, 0), _fallbackSprite(props.fallbackSprite) {
        BindProp(props.src, props.sSrc, [this](std::string const& v) { SetSrc(v); });
    }

    void SetSrc(std::string url);
    std::string const& GetSrc() const { return _src; }

    void SetFallbackSprite(UnityEngine::Sprite* s) { _fallbackSprite = s; }

    // Called when loading fails, after the fallback is shown.
    std::function<void()> OnLoadFailed;

protected:
    void OnInitialize() override;

private:
    std::string          _src;
    UnityEngine::Sprite* _fallbackSprite{nullptr};
};
}
