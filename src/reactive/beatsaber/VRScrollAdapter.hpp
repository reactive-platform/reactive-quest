#pragma once

#include <GlobalNamespace/IVRPlatformHelper.hpp>
#include <UnityEngine/MonoBehaviour.hpp>

#include "custom-types/shared/macros.hpp"

#include "reactive/components/PointerEventsHandler.hpp"

DECLARE_CLASS_CODEGEN(Reactive::BeatSaber, VRScrollAdapter, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(Reactive::PointerEventsHandler*,       _pointerEventsHandler);
    DECLARE_INSTANCE_FIELD(::GlobalNamespace::IVRPlatformHelper*, _platformHelper);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Update);
};
