#include "reactive/beatsaber/WebImage.hpp"

#include "reactive/components/ImageLoader.hpp"

namespace reactive::beatsaber {
namespace {
    // Unity draws an Image without a sprite as a solid rectangle, so showing nothing takes a transparent sprite.
    UnityEngine::Sprite* TransparentSprite() {
        auto* sprites = Sprites();
        return sprites ? sprites->transparentPixel : nullptr;
    }
}

void WebImage::OnInitialize() {
    if (!GetSprite()) SetSprite(TransparentSprite());
}

void WebImage::SetSrc(std::string url) {
    if (url == _src) return;
    _src = std::move(url);

    if (_src.empty()) {
        SetSprite(TransparentSprite());
        return;
    }

    components::LoadImage(_src, [this, lifetime = LifetimeToken(), src = _src](UnityEngine::Sprite* sprite) {
        if (lifetime.expired() || _src != src) return;

        if (sprite) {
            SetSprite(sprite);
            return;
        }
        SetSprite(_fallbackSprite ? _fallbackSprite : TransparentSprite());
        if (OnLoadFailed) OnLoadFailed();
    });
}
}
