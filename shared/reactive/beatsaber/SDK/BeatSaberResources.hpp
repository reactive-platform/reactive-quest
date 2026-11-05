#pragma once

#include <UnityEngine/ScriptableObject.hpp>

#include "custom-types/shared/macros.hpp"

#include "reactive/beatsaber/SDK/MaterialCollection.hpp"
#include "reactive/beatsaber/SDK/SpriteCollection.hpp"

DECLARE_CLASS_CODEGEN(Reactive::BeatSaber, BeatSaberResources, UnityEngine::ScriptableObject) {
    DECLARE_INSTANCE_FIELD(Reactive::BeatSaber::SpriteCollection*,   sprites);
    DECLARE_INSTANCE_FIELD(Reactive::BeatSaber::MaterialCollection*, materials);
};
