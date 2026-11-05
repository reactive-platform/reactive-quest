#pragma once

#include <TMPro/TMP_FontAsset.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/Sprite.hpp>

// Quest has no editor-baked instance, so resources are looked up at runtime. Accessors return null until the game has loaded them.
namespace reactive::beatsaber {
UnityEngine::Material* UIFontMaterial();
UnityEngine::Material* UINoGlowMaterial();
UnityEngine::Material* UIFogBackgroundMaterial();
TMPro::TMP_FontAsset*  MainTextFont();
UnityEngine::Sprite*   ArrowIcon();
UnityEngine::Sprite*   VerticalIndicatorIcon();

void RaiseButtonClickSignal();
}
