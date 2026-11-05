#include "reactive/beatsaber/GameResources.hpp"

#include <string_view>

#include <GlobalNamespace/Signal.hpp>
#include <UnityEngine/Resources.hpp>

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

namespace reactive::beatsaber {
namespace {
    template <class T>
    T* FindByName(SafePtrUnity<T>& cache, std::string_view name) {
        if (cache) return cache.ptr();

        auto all = UnityEngine::Resources::FindObjectsOfTypeAll<T*>();
        for (size_t i = 0; i < all.size(); ++i) {
            if (all[i] && static_cast<std::string>(all[i]->get_name()) == name) {
                cache = all[i];
                return all[i];
            }
        }
        return nullptr;
    }
}

#define GAME_RESOURCE(Type, Accessor, assetName)   \
    Type* Accessor() {                             \
        static SafePtrUnity<Type> cache;           \
        return FindByName(cache, assetName);       \
    }

GAME_RESOURCE(UnityEngine::Material, UIFontMaterial, "Teko-Medium SDF Curved Softer")
GAME_RESOURCE(UnityEngine::Material, UINoGlowMaterial, "UINoGlow")
GAME_RESOURCE(UnityEngine::Material, UIFogBackgroundMaterial, "UIFogBG")
GAME_RESOURCE(TMPro::TMP_FontAsset, MainTextFont, "Teko-Medium SDF")
GAME_RESOURCE(UnityEngine::Sprite, ArrowIcon, "ArrowIcon")
GAME_RESOURCE(UnityEngine::Sprite, VerticalIndicatorIcon, "VerticalRoundRect8")
static GAME_RESOURCE(GlobalNamespace::Signal, ButtonClickSignal, "UIButtonWasPressed")

#undef GAME_RESOURCE

void RaiseButtonClickSignal() {
    if (auto* signal = ButtonClickSignal()) signal->Raise();
}
}
