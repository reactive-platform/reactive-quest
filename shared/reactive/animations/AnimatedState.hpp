#pragma once

#include <functional>
#include <utility>

#include <UnityEngine/Time.hpp>

#include "reactive/animations/AnimationCurve.hpp"
#include "reactive/animations/AnimationDuration.hpp"
#include "reactive/animations/AnimationMath.hpp"
#include "reactive/animations/Interpolators.hpp"
#include "reactive/animations/Modules.hpp"
#include "reactive/state/State.hpp"

namespace reactive {
template <class T>
class AnimatedState final : public StateBase<T>, public IReactiveModule {
public:
    AnimatedState(T initialValue, IValueInterpolator<T>* interpolator)
        : _interpolator(interpolator), _start(initialValue), _target(initialValue), _value(std::move(initialValue)) {}

    T const& Value() const override { return _value; }

    T const& TargetValue() const { return _target; }
    void     SetTargetValue(T value) {
        if (_interpolator->Equals(_target, value)) return;

        _start  = _value;
        _target = std::move(value);

        _elapsed        = 0.f;
        _progress       = 0.f;
        _linearProgress = 0.f;
        _set            = false;
    }

    float Progress() const { return _progress; }
    bool  IsFinished() const { return _set; }

    AnimationDuration                Duration{};
    AnimationCurve*                  Curve{AnimationCurve::Linear()};
    std::function<void(T const&)>    OnFinish;

    void SetValueImmediate(T value, bool silent = false) {
        _set            = true;
        _start          = value;
        _target         = value;
        _linearProgress = 1.f;
        _progress       = 1.f;
        _value          = std::move(value);
        if (!silent) {
            this->NotifyValueInvalidated();
            FinishAnimation();
        }
    }

    void EvaluateNextFrame() { _set = false; }

    void OnUpdate() override {
        if (_set) return;

        const float dt = UnityEngine::Time::get_deltaTime();
        if (Duration.unit == DurationUnit::Seconds) {
            _elapsed += dt;
            _linearProgress = Duration.value <= 0.f ? 1.f : anim::Clamp01(_elapsed / Duration.value);
        } else {
            _linearProgress = anim::Lerp(_linearProgress, 1.f, dt * Duration.value);
            // Factor-based progress only approaches 1, so snap it at the end.
            if (anim::Approximately(1.f, _linearProgress)) _linearProgress = 1.f;
        }

        SetProgress(Curve->Evaluate(_linearProgress));

        if (_linearProgress >= 1.f) FinishAnimation();
    }

private:
    void SetProgress(float p) {
        _progress = p;
        _value    = _interpolator->Lerp(_start, _target, p);
        this->NotifyValueInvalidated();
    }

    void FinishAnimation() {
        _set     = true;
        _elapsed = 0.f;
        if (OnFinish) OnFinish(_value);
    }

    IValueInterpolator<T>* _interpolator;
    T _start;
    T _target;
    T _value;

    float _progress{1.f};
    float _linearProgress{1.f};
    float _elapsed{0.f};
    bool  _set{true};
};
}
