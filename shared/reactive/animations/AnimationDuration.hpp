#pragma once

namespace reactive {
enum class DurationUnit {
    Seconds,
    TimeDeltaFactor,
};

struct AnimationDuration {
    float       value{0.f};
    DurationUnit unit{DurationUnit::Seconds};
};

inline AnimationDuration sec(float time)  { return {time, DurationUnit::Seconds}; }
inline AnimationDuration ms(float time)   { return {time / 1000.f, DurationUnit::Seconds}; }
inline AnimationDuration fact(float factor) { return {factor, DurationUnit::TimeDeltaFactor}; }
}
