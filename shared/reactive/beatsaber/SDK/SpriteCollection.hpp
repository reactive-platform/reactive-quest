#pragma once

#include <UnityEngine/ScriptableObject.hpp>
#include <UnityEngine/Sprite.hpp>

#include "custom-types/shared/macros.hpp"

// Field names must match the C# source, because Unity's deserializer matches them by name.
DECLARE_CLASS_CODEGEN(Reactive::BeatSaber, SpriteCollection, UnityEngine::ScriptableObject) {
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, rectangle);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, background);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, backgroundLeft);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, backgroundRight);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, backgroundBottom);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, backgroundTop);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, backgroundUnderline);

    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, spinnerIcon);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, crossIcon);

    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, glare);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, frame);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, transparentPixel);
};
