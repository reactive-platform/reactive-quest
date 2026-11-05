#pragma once

#include <yoga/Yoga.h>

namespace reactive {
enum class Align {
    Auto         = YGAlignAuto,
    FlexStart    = YGAlignFlexStart,
    Center       = YGAlignCenter,
    FlexEnd      = YGAlignFlexEnd,
    Stretch      = YGAlignStretch,
    Baseline     = YGAlignBaseline,
    SpaceBetween = YGAlignSpaceBetween,
    SpaceAround  = YGAlignSpaceAround,
    SpaceEvenly  = YGAlignSpaceEvenly,
};

enum class Direction {
    Inherit     = YGDirectionInherit,
    LeftToRight = YGDirectionLTR,
    RightToLeft = YGDirectionRTL,
};

enum class FlexDirection {
    Column        = YGFlexDirectionColumn,
    ColumnReverse = YGFlexDirectionColumnReverse,
    Row           = YGFlexDirectionRow,
    RowReverse    = YGFlexDirectionRowReverse,
};

enum class Justify {
    FlexStart    = YGJustifyFlexStart,
    Center       = YGJustifyCenter,
    FlexEnd      = YGJustifyFlexEnd,
    SpaceBetween = YGJustifySpaceBetween,
    SpaceAround  = YGJustifySpaceAround,
    SpaceEvenly  = YGJustifySpaceEvenly,
};

enum class Overflow {
    Visible = YGOverflowVisible,
    Hidden  = YGOverflowHidden,
};

enum class PositionType {
    Static   = YGPositionTypeStatic,
    Relative = YGPositionTypeRelative,
    Absolute = YGPositionTypeAbsolute,
};

// PC also has MaxContent, FitContent and Stretch, which this Yoga version predates.
enum class Unit {
    Undefined = YGUnitUndefined,
    Point     = YGUnitPoint,
    Percent   = YGUnitPercent,
    Auto      = YGUnitAuto,
};

enum class Wrap {
    NoWrap      = YGWrapNoWrap,
    Wrap        = YGWrapWrap,
    WrapReverse = YGWrapWrapReverse,
};

enum class MeasureMode {
    Undefined = YGMeasureModeUndefined,
    Exactly   = YGMeasureModeExactly,
    AtMost    = YGMeasureModeAtMost,
};
}
