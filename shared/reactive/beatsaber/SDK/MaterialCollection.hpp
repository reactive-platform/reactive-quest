#pragma once

#include <UnityEngine/Material.hpp>
#include <UnityEngine/ScriptableObject.hpp>

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Reactive::BeatSaber, MaterialCollection, UnityEngine::ScriptableObject) {
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, uiAdditiveGlowMaterial);
};
