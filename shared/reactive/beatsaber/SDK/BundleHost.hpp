#pragma once

#include <UnityEngine/MonoBehaviour.hpp>

#include "custom-types/shared/macros.hpp"

#include "reactive/beatsaber/SDK/BeatSaberResources.hpp"

// A raw C++ pointer doesn't count as a GC root, so the bundle's ScriptableObject is rooted here.
DECLARE_CLASS_CODEGEN(Reactive::BeatSaber, BundleHost, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(Reactive::BeatSaber::BeatSaberResources*, resources);
};
