#pragma once

#include <UnityEngine/Color.hpp>
#include <UnityEngine/Vector2.hpp>
#include <UnityEngine/Vector3.hpp>

#include "reactive/animations/AnimationMath.hpp"

namespace reactive {
template <class T>
class IValueInterpolator {
public:
    virtual ~IValueInterpolator() = default;
    virtual T    Lerp(T from, T to, float t) const = 0;
    virtual bool Equals(T x, T y) const             = 0;
};

class SingleValueInterpolator : public IValueInterpolator<float> {
public:
    float Lerp(float from, float to, float t) const override {
        return anim::LerpUnclamped(from, to, t);
    }
    bool Equals(float x, float y) const override { return anim::Approximately(x, y); }

    static SingleValueInterpolator* Instance() {
        static SingleValueInterpolator i;
        return &i;
    }
};

class ColorValueInterpolator : public IValueInterpolator<UnityEngine::Color> {
public:
    UnityEngine::Color Lerp(UnityEngine::Color from, UnityEngine::Color to, float t) const override {
        return anim::LerpUnclamped(from, to, t);
    }
    bool Equals(UnityEngine::Color x, UnityEngine::Color y) const override {
        return anim::Approximately(x.r, y.r) && anim::Approximately(x.g, y.g) &&
               anim::Approximately(x.b, y.b) && anim::Approximately(x.a, y.a);
    }

    static ColorValueInterpolator* Instance() {
        static ColorValueInterpolator i;
        return &i;
    }
};

class Vector2ValueInterpolator : public IValueInterpolator<UnityEngine::Vector2> {
public:
    UnityEngine::Vector2 Lerp(UnityEngine::Vector2 from, UnityEngine::Vector2 to, float t) const override {
        return anim::LerpUnclamped(from, to, t);
    }
    bool Equals(UnityEngine::Vector2 x, UnityEngine::Vector2 y) const override {
        return anim::Approximately(x.x, y.x) && anim::Approximately(x.y, y.y);
    }

    static Vector2ValueInterpolator* Instance() {
        static Vector2ValueInterpolator i;
        return &i;
    }
};

class Vector3ValueInterpolator : public IValueInterpolator<UnityEngine::Vector3> {
public:
    UnityEngine::Vector3 Lerp(UnityEngine::Vector3 from, UnityEngine::Vector3 to, float t) const override {
        return anim::LerpUnclamped(from, to, t);
    }
    bool Equals(UnityEngine::Vector3 x, UnityEngine::Vector3 y) const override {
        return anim::Approximately(x.x, y.x) && anim::Approximately(x.y, y.y) &&
               anim::Approximately(x.z, y.z);
    }

    static Vector3ValueInterpolator* Instance() {
        static Vector3ValueInterpolator i;
        return &i;
    }
};
}
