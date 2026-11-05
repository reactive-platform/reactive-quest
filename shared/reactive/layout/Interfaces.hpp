#pragma once

#include <UnityEngine/Vector2.hpp>

#include "reactive/yoga/Enums.hpp"

namespace reactive {
struct LayoutDimensions {
    float left;
    float top;
    float width;
    float height;

    bool operator==(LayoutDimensions const&) const = default;
};

class ILeafLayoutItem {
public:
    virtual ~ILeafLayoutItem() = default;

    virtual UnityEngine::Vector2 Measure(float width, MeasureMode widthMode, float height, MeasureMode heightMode) = 0;
};
}
