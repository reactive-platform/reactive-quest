#pragma once

#include <cmath>

#include "reactive/animations/AnimationMath.hpp"

namespace reactive {
// Presets are accessors rather than PC's static fields, to avoid static init order problems between translation units.
class AnimationCurve {
public:
    virtual ~AnimationCurve() = default;

    virtual float Evaluate(float progress) const { return progress; }

    static AnimationCurve* Linear();
    static AnimationCurve* Exponential();
    static AnimationCurve* EaseIn();
    static AnimationCurve* EaseOut();
    static AnimationCurve* EaseInOut();
    static AnimationCurve* EaseInQuint();
    static AnimationCurve* EaseOutQuint();
    static AnimationCurve* EaseInOutQuint();
    static AnimationCurve* EaseInExpo();
    static AnimationCurve* EaseOutExpo();
    static AnimationCurve* EaseInOutExpo();
    static AnimationCurve* EaseInBack();
    static AnimationCurve* EaseOutBack();
    static AnimationCurve* EaseInOutBack();
    static AnimationCurve* EaseInSine();
    static AnimationCurve* EaseOutSine();
    static AnimationCurve* EaseInOutSine();
    static AnimationCurve* EaseInQuad();
    static AnimationCurve* EaseOutQuad();
    static AnimationCurve* EaseInOutQuad();
    static AnimationCurve* EaseInCubic();
    static AnimationCurve* EaseOutCubic();
    static AnimationCurve* EaseInOutCubic();
};

class EaseInCurve : public AnimationCurve {
public:
    float Evaluate(float progress) const override { return progress * progress; }
};

class EaseOutCurve : public AnimationCurve {
public:
    float Evaluate(float progress) const override {
        float inv = 1.f - progress;
        return 1.f - inv * inv;
    }
};

class EaseInOutCurve : public AnimationCurve {
public:
    float Evaluate(float progress) const override {
        if (progress < 0.5f) return 2.f * progress * progress;
        float v = -2.f * progress + 2.f;
        return 1.f - (v * v) / 2.f;
    }
};

class ExponentialCurve : public AnimationCurve {
public:
    float Evaluate(float progress) const override {
        static const float infimum  = 1.f - std::exp(-0.f);
        static const float supremum = 1.f - std::exp(-1.f);
        return anim::Map(1.f - std::exp(-progress), infimum, supremum, 0.f, 1.f);
    }
};

class BezierCurve : public AnimationCurve {
public:
    BezierCurve(float x1, float y1, float x2, float y2)
        : _x1(x1), _y1(y1), _x2(x2), _y2(y2) {}

    float Evaluate(float progress) const override {
        float t = Solve(progress, 1e-5f, 10);
        return Bezier(t, 0.f, _y1, _y2, 1.f);
    }

private:
    float _x1, _y1, _x2, _y2;

    static float Bezier(float t, float p0, float p1, float p2, float p3) {
        float c = 1.f - t;
        return c * c * c * p0 + 3.f * c * c * t * p1 + 3.f * c * t * t * p2 + t * t * t * p3;
    }

    static float BezierDerivative(float t, float p0, float p1, float p2, float p3) {
        float c = 1.f - t;
        return 3.f * c * c * (p1 - p0) + 6.f * c * t * (p2 - p1) + 3.f * t * t * (p3 - p2);
    }

    float Solve(float progress, float epsilon, int maxIter) const {
        float t = progress;
        for (int i = 0; i < maxIter; ++i) {
            float x = Bezier(t, 0.f, _x1, _x2, 1.f) - progress;
            if (std::fabs(x) < epsilon) break;
            float dx = BezierDerivative(t, 0.f, _x1, _x2, 1.f);
            if (std::fabs(dx) < 1e-6f) break;
            t -= x / dx;
        }
        return anim::Clamp(t, 0.f, 1.f);
    }
};

inline AnimationCurve* AnimationCurve::Linear()      { static AnimationCurve   c;                            return &c; }
inline AnimationCurve* AnimationCurve::Exponential() { static ExponentialCurve c;                            return &c; }
inline AnimationCurve* AnimationCurve::EaseIn()      { static EaseInCurve      c;                            return &c; }
inline AnimationCurve* AnimationCurve::EaseOut()     { static EaseOutCurve     c;                            return &c; }
inline AnimationCurve* AnimationCurve::EaseInOut()   { static EaseInOutCurve   c;                            return &c; }
inline AnimationCurve* AnimationCurve::EaseInQuint()    { static BezierCurve c(0.64f, 0.0f, 0.78f, 0.0f);    return &c; }
inline AnimationCurve* AnimationCurve::EaseOutQuint()   { static BezierCurve c(0.22f, 1.0f, 0.36f, 1.0f);    return &c; }
inline AnimationCurve* AnimationCurve::EaseInOutQuint() { static BezierCurve c(0.83f, 0.0f, 0.17f, 1.0f);    return &c; }
inline AnimationCurve* AnimationCurve::EaseInExpo()     { static BezierCurve c(0.95f, 0.05f, 0.795f, 0.035f);return &c; }
inline AnimationCurve* AnimationCurve::EaseOutExpo()    { static BezierCurve c(0.19f, 1.0f, 0.22f, 1.0f);    return &c; }
inline AnimationCurve* AnimationCurve::EaseInOutExpo()  { static BezierCurve c(1.0f, 0.0f, 0.0f, 1.0f);      return &c; }
inline AnimationCurve* AnimationCurve::EaseInBack()     { static BezierCurve c(0.6f, -0.28f, 0.735f, 0.045f);return &c; }
inline AnimationCurve* AnimationCurve::EaseOutBack()    { static BezierCurve c(0.175f, 0.885f, 0.32f, 1.275f);return &c; }
inline AnimationCurve* AnimationCurve::EaseInOutBack()  { static BezierCurve c(0.68f, -0.55f, 0.265f, 1.55f);return &c; }
inline AnimationCurve* AnimationCurve::EaseInSine()     { static BezierCurve c(0.47f, 0.0f, 0.745f, 0.715f); return &c; }
inline AnimationCurve* AnimationCurve::EaseOutSine()    { static BezierCurve c(0.39f, 0.575f, 0.565f, 1.0f); return &c; }
inline AnimationCurve* AnimationCurve::EaseInOutSine()  { static BezierCurve c(0.445f, 0.05f, 0.55f, 0.95f); return &c; }
inline AnimationCurve* AnimationCurve::EaseInQuad()     { static BezierCurve c(0.55f, 0.085f, 0.68f, 0.53f); return &c; }
inline AnimationCurve* AnimationCurve::EaseOutQuad()    { static BezierCurve c(0.25f, 0.46f, 0.45f, 0.94f);  return &c; }
inline AnimationCurve* AnimationCurve::EaseInOutQuad()  { static BezierCurve c(0.455f, 0.03f, 0.515f, 0.955f);return &c; }
inline AnimationCurve* AnimationCurve::EaseInCubic()    { static BezierCurve c(0.55f, 0.055f, 0.675f, 0.19f);return &c; }
inline AnimationCurve* AnimationCurve::EaseOutCubic()   { static BezierCurve c(0.215f, 0.61f, 0.355f, 1.0f); return &c; }
inline AnimationCurve* AnimationCurve::EaseInOutCubic() { static BezierCurve c(0.645f, 0.045f, 0.355f, 1.0f);return &c; }
}
