#pragma once

#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Vector2.hpp>

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Reactive::BeatSaber, RaycastMask2D, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, _rectTransform);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(bool, ValidatePoint,      UnityEngine::Vector2 point);
    DECLARE_INSTANCE_METHOD(bool, ValidateLocalPoint, UnityEngine::Vector2 point);
};
