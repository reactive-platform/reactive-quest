#include "reactive/beatsaber/ScrollArea.hpp"

#include <UnityEngine/GameObject.hpp>

#include "reactive/beatsaber/VRScrollAdapter.hpp"

namespace reactive::beatsaber {
void ScrollArea::OnInitialize() {
    Content()->AddComponent<Reactive::BeatSaber::VRScrollAdapter*>();
    ::reactive::components::ScrollArea::OnInitialize();
}
}
