#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <UnityEngine/Color.hpp>
#include <UnityEngine/Vector2.hpp>
#include <UnityEngine/Vector3.hpp>

namespace reactive::anim {
inline float Clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

inline float Clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

inline float LerpUnclamped(float from, float to, float t) {
    return from + (to - from) * t;
}

inline float Lerp(float from, float to, float t) {
    return LerpUnclamped(from, to, Clamp01(t));
}

// Mathf.Approximately
inline bool Approximately(float a, float b) {
    return std::fabs(b - a) <
           std::max(1e-6f * std::max(std::fabs(a), std::fabs(b)), std::numeric_limits<float>::min() * 8.f);
}

inline float Map(float value, float inMin, float inMax, float outMin, float outMax) {
    if (inMax - inMin == 0.f) return outMin;
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

inline UnityEngine::Color LerpUnclamped(UnityEngine::Color a, UnityEngine::Color b, float t) {
    return UnityEngine::Color{
        LerpUnclamped(a.r, b.r, t),
        LerpUnclamped(a.g, b.g, t),
        LerpUnclamped(a.b, b.b, t),
        LerpUnclamped(a.a, b.a, t),
    };
}

inline UnityEngine::Vector2 LerpUnclamped(UnityEngine::Vector2 a, UnityEngine::Vector2 b, float t) {
    return UnityEngine::Vector2{LerpUnclamped(a.x, b.x, t), LerpUnclamped(a.y, b.y, t)};
}

inline UnityEngine::Vector3 LerpUnclamped(UnityEngine::Vector3 a, UnityEngine::Vector3 b, float t) {
    return UnityEngine::Vector3{
        LerpUnclamped(a.x, b.x, t),
        LerpUnclamped(a.y, b.y, t),
        LerpUnclamped(a.z, b.z, t),
    };
}
}
