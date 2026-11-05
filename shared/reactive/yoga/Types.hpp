#pragma once

#include <limits>
#include <yoga/Yoga.h>

#include "reactive/yoga/Enums.hpp"

namespace reactive {
struct YogaValue {
    float value{0.f};
    Unit  unit{Unit::Undefined};

    constexpr YogaValue() = default;
    constexpr YogaValue(float v, Unit u) : value(v), unit(u) {}

    constexpr YogaValue(float v) : value(v), unit(Unit::Point) {}
    constexpr YogaValue(double v) : value(static_cast<float>(v)), unit(Unit::Point) {}
    constexpr YogaValue(int v)   : value(static_cast<float>(v)), unit(Unit::Point) {}

    static const YogaValue Zero;
    static const YogaValue Auto;

    constexpr bool IsUndefined() const { return unit == Unit::Undefined; }

    constexpr YogaValue operator-() const { return {-value, unit}; }
};

inline constexpr YogaValue YogaValue::Zero      {0.f, Unit::Point};
inline constexpr YogaValue YogaValue::Auto      {std::numeric_limits<float>::quiet_NaN(), Unit::Auto};

inline namespace literals {
constexpr YogaValue operator""_pt(long double v)         { return {static_cast<float>(v), Unit::Point}; }
constexpr YogaValue operator""_pt(unsigned long long v)  { return {static_cast<float>(v), Unit::Point}; }
constexpr YogaValue operator""_pct(long double v)        { return {static_cast<float>(v), Unit::Percent}; }
constexpr YogaValue operator""_pct(unsigned long long v) { return {static_cast<float>(v), Unit::Percent}; }
}

struct YogaVector {
    YogaValue x{};
    YogaValue y{};

    static constexpr YogaVector All(YogaValue v) { return {v, v}; }

    constexpr bool IsUndefined() const { return x.IsUndefined() && y.IsUndefined(); }
};

struct YogaFrame {
    YogaValue top{};
    YogaValue bottom{};
    YogaValue left{};
    YogaValue right{};

    static constexpr YogaFrame All(YogaValue v) { return {v, v, v, v}; }
    static constexpr YogaFrame Symmetric(YogaValue vertical, YogaValue horizontal) {
        return {vertical, vertical, horizontal, horizontal};
    }

    static const YogaFrame Auto;

    constexpr bool IsUndefined() const {
        return top.IsUndefined() && bottom.IsUndefined() && left.IsUndefined() && right.IsUndefined();
    }
};

inline constexpr YogaFrame YogaFrame::Auto = YogaFrame::All(YogaValue::Auto);
}
