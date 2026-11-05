#pragma once

#include <UnityEngine/AssetBundle.hpp>
#include <UnityEngine/GameObject.hpp>

#include "custom-types/shared/coroutine.hpp"

#include "reactive/beatsaber/SDK/BundleHost.hpp"

#define DECLARE_FILE(name, prefix) extern "C" uint8_t _binary_##name##_start[]; extern "C" uint8_t _binary_##name##_end[]; struct prefix##name { static size_t getLength() { return _binary_##name##_end - _binary_##name##_start; } static uint8_t* getData() { return _binary_##name##_start; } };
DECLARE_FILE(reactive_bundle,)

namespace reactive::beatsaber {
class BundleLoader {
public:
    static UnityEngine::AssetBundle*        assetBundle;
    static Reactive::BeatSaber::BundleHost* host;

    static custom_types::Helpers::Coroutine LoadBundle(UnityEngine::GameObject* container);
};

inline Reactive::BeatSaber::BeatSaberResources* Resources() {
    return (BundleLoader::host && BundleLoader::host->resources)
               ? BundleLoader::host->resources
               : nullptr;
}
inline Reactive::BeatSaber::SpriteCollection* Sprites() {
    auto* r = Resources();
    return r ? r->sprites : nullptr;
}
inline Reactive::BeatSaber::MaterialCollection* Materials() {
    auto* r = Resources();
    return r ? r->materials : nullptr;
}
}
