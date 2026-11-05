#pragma once

#include <UnityEngine/MonoBehaviour.hpp>

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Reactive, ReactiveHost, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(int64_t, _native);

    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, OnRectTransformDimensionsChange);
};
