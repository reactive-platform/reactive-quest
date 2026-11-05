#pragma once

#include <UnityEngine/Color.hpp>

#include "reactive/beatsaber/CompositeColorSet.hpp"

namespace reactive::beatsaber {
struct BsButtonColors {
    CompositeColorSet backgroundColors;
    ColorSet          contentColors;
    ColorSet          underlineColors;
};

inline namespace BeatSaberStyle {
constexpr float Skew = 0.18f;

inline const BsButtonColors ButtonColors = {
    .backgroundColors = {
        .colors = {
            .color                = {0.f, 0.f, 0.f, 0.5f},
            .hoveredColor         = UnityEngine::Color{1.f, 1.f, 1.f, 0.3f},
            .activeColor          = UnityEngine::Color{1.f, 1.f, 1.f, 0.5f},
            .notInteractableColor = UnityEngine::Color{0.f, 0.f, 0.f, 0.25f},
        },
        .gradientColors0 = ColorSet::White(),
        .gradientColors1 = {
            .color        = {1.f, 1.f, 1.f, 1.f},
            .hoveredColor = UnityEngine::Color{1.f, 1.f, 1.f, 0.5f},
        },
    },
    .contentColors = {
        .color        = {1.f, 1.f, 1.f, 0.75f},
        .hoveredColor = UnityEngine::Color{1.f, 1.f, 1.f, 1.f},
    },
    .underlineColors = {
        .color = {1.f, 1.f, 1.f, 0.5f},
    },
};

constexpr UnityEngine::Color TextColor          = {1.f, 1.f, 1.f, 1.f};
constexpr UnityEngine::Color SecondaryTextColor = {0.9f, 0.9f, 0.9f, 0.9f};
}
}
