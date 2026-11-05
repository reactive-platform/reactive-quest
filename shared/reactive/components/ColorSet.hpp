#pragma once

#include <memory>
#include <optional>

#include <UnityEngine/Color.hpp>

#include "reactive/components/GraphicState.hpp"
#include "reactive/state/State.hpp"

namespace reactive {
struct ColorSet {
    UnityEngine::Color                color{};
    std::optional<UnityEngine::Color> hoveredColor;
    std::optional<UnityEngine::Color> activeColor;
    std::optional<UnityEngine::Color> hoveredActiveColor;
    std::optional<UnityEngine::Color> notInteractableColor;
    std::optional<UnityEngine::Color> notInteractableHoveredColor;
    std::optional<UnityEngine::Color> notInteractableActiveColor;
    std::optional<UnityEngine::Color> notInteractableHoveredActiveColor;

    UnityEngine::Color GetColor(GraphicState state) const {
        const bool hovered = IsHovered(state);
        const bool active  = IsActive(state);
        if (!IsInteractable(state)) {
            if (hovered && active) {
                return notInteractableHoveredActiveColor.value_or(
                    notInteractableActiveColor.value_or(
                        notInteractableHoveredColor.value_or(notInteractableColor.value_or(color))));
            }
            if (active)  return notInteractableActiveColor.value_or(notInteractableColor.value_or(color));
            if (hovered) return notInteractableHoveredColor.value_or(notInteractableColor.value_or(color));
            return notInteractableColor.value_or(color);
        }
        if (hovered && active) {
            return hoveredActiveColor.value_or(activeColor.value_or(hoveredColor.value_or(color)));
        }
        if (active)  return activeColor.value_or(color);
        if (hovered) return hoveredColor.value_or(color);
        return color;
    }

    static ColorSet White() { return {.color = {1, 1, 1, 1}}; }
};

template <StateType S>
    requires std::same_as<typename S::value_type, GraphicState>
auto MapColorSet(std::shared_ptr<S> const& state, ColorSet set) {
    return state->Map([set](GraphicState s) { return set.GetColor(s); });
}

inline void SetHovered(State<GraphicState>& s, bool v) {
    s.SetValue(Set(s.Value(), GraphicState::Hovered, v));
}
inline void SetActive(State<GraphicState>& s, bool v) {
    s.SetValue(Set(s.Value(), GraphicState::Active, v));
}
inline void SetInteractable(State<GraphicState>& s, bool v) {
    s.SetValue(Set(s.Value(), GraphicState::NonInteractable, !v));
}
}
