#pragma once

#include <yoga/Yoga.h>

#include "reactive/yoga/Types.hpp"

namespace reactive::yogastyle {
inline void Width(YGNodeRef n, YogaValue v) {
    switch (v.unit) {
        case Unit::Point:   YGNodeStyleSetWidth(n, v.value); break;
        case Unit::Percent: YGNodeStyleSetWidthPercent(n, v.value); break;
        case Unit::Auto:    YGNodeStyleSetWidthAuto(n); break;
        default:            YGNodeStyleSetWidth(n, YGUndefined); break;
    }
}

inline void Height(YGNodeRef n, YogaValue v) {
    switch (v.unit) {
        case Unit::Point:   YGNodeStyleSetHeight(n, v.value); break;
        case Unit::Percent: YGNodeStyleSetHeightPercent(n, v.value); break;
        case Unit::Auto:    YGNodeStyleSetHeightAuto(n); break;
        default:            YGNodeStyleSetHeight(n, YGUndefined); break;
    }
}

inline void MinWidth(YGNodeRef n, YogaValue v) {
    if (v.unit == Unit::Percent) YGNodeStyleSetMinWidthPercent(n, v.value);
    else YGNodeStyleSetMinWidth(n, v.unit == Unit::Point ? v.value : YGUndefined);
}

inline void MinHeight(YGNodeRef n, YogaValue v) {
    if (v.unit == Unit::Percent) YGNodeStyleSetMinHeightPercent(n, v.value);
    else YGNodeStyleSetMinHeight(n, v.unit == Unit::Point ? v.value : YGUndefined);
}

inline void MaxWidth(YGNodeRef n, YogaValue v) {
    if (v.unit == Unit::Percent) YGNodeStyleSetMaxWidthPercent(n, v.value);
    else YGNodeStyleSetMaxWidth(n, v.unit == Unit::Point ? v.value : YGUndefined);
}

inline void MaxHeight(YGNodeRef n, YogaValue v) {
    if (v.unit == Unit::Percent) YGNodeStyleSetMaxHeightPercent(n, v.value);
    else YGNodeStyleSetMaxHeight(n, v.unit == Unit::Point ? v.value : YGUndefined);
}

inline void FlexBasis(YGNodeRef n, YogaValue v) {
    switch (v.unit) {
        case Unit::Point:   YGNodeStyleSetFlexBasis(n, v.value); break;
        case Unit::Percent: YGNodeStyleSetFlexBasisPercent(n, v.value); break;
        case Unit::Auto:    YGNodeStyleSetFlexBasisAuto(n); break;
        default:            YGNodeStyleSetFlexBasis(n, YGUndefined); break;
    }
}

namespace detail {
    inline void Edges(YogaFrame const& f, auto&& set) {
        set(YGEdgeTop, f.top);
        set(YGEdgeBottom, f.bottom);
        set(YGEdgeLeft, f.left);
        set(YGEdgeRight, f.right);
    }
}

inline void Margin(YGNodeRef n, YogaFrame const& f) {
    detail::Edges(f, [n](YGEdge e, YogaValue v) {
        switch (v.unit) {
            case Unit::Point:   YGNodeStyleSetMargin(n, e, v.value); break;
            case Unit::Percent: YGNodeStyleSetMarginPercent(n, e, v.value); break;
            case Unit::Auto:    YGNodeStyleSetMarginAuto(n, e); break;
            default:            YGNodeStyleSetMargin(n, e, YGUndefined); break;
        }
    });
}

inline void Position(YGNodeRef n, YogaFrame const& f) {
    detail::Edges(f, [n](YGEdge e, YogaValue v) {
        switch (v.unit) {
            case Unit::Point:   YGNodeStyleSetPosition(n, e, v.value); break;
            case Unit::Percent: YGNodeStyleSetPositionPercent(n, e, v.value); break;
            case Unit::Auto:    YGNodeStyleSetPositionAuto(n, e); break;
            default:            YGNodeStyleSetPosition(n, e, YGUndefined); break;
        }
    });
}

inline void Padding(YGNodeRef n, YogaFrame const& f) {
    detail::Edges(f, [n](YGEdge e, YogaValue v) {
        if (v.unit == Unit::Percent) YGNodeStyleSetPaddingPercent(n, e, v.value);
        else YGNodeStyleSetPadding(n, e, v.unit == Unit::Point ? v.value : YGUndefined);
    });
}

inline void Gap(YGNodeRef n, YogaVector const& g) {
    auto set = [n](YGGutter gutter, YogaValue v) {
        if (v.unit == Unit::Percent) YGNodeStyleSetGapPercent(n, gutter, v.value);
        else YGNodeStyleSetGap(n, gutter, v.unit == Unit::Point ? v.value : YGUndefined);
    };
    set(YGGutterColumn, g.x);
    set(YGGutterRow, g.y);
}
}
