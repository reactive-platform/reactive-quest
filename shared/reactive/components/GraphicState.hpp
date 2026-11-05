#pragma once

#include <cstdint>

namespace reactive {
enum class GraphicState : uint32_t {
    None            = 0,
    Hovered         = 1,
    Active          = 2,
    NonInteractable = 4,
};

inline bool IsInteractable(GraphicState s) { return (static_cast<uint32_t>(s) & static_cast<uint32_t>(GraphicState::NonInteractable)) == 0; }
inline bool IsHovered(GraphicState s)      { return (static_cast<uint32_t>(s) & static_cast<uint32_t>(GraphicState::Hovered)) > 0; }
inline bool IsActive(GraphicState s)       { return (static_cast<uint32_t>(s) & static_cast<uint32_t>(GraphicState::Active)) > 0; }

inline GraphicState Set(GraphicState state, GraphicState mask, bool value) {
    auto s = static_cast<uint32_t>(state);
    auto m = static_cast<uint32_t>(mask);
    return static_cast<GraphicState>(value ? (s | m) : (s & ~m));
}
}
