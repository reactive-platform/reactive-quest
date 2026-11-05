#pragma once

#include <UnityEngine/Color.hpp>

#include "reactive/components/ColorSet.hpp"

namespace reactive::beatsaber {
struct CompositeColors {
    UnityEngine::Color color;
    UnityEngine::Color gradientColor0;
    UnityEngine::Color gradientColor1;
};

struct CompositeColorSet {
    ColorSet colors;
    ColorSet gradientColors0;
    ColorSet gradientColors1;

    CompositeColors GetColors(GraphicState state) const {
        return {colors.GetColor(state), gradientColors0.GetColor(state), gradientColors1.GetColor(state)};
    }
};
}
