#include "reactive/beatsaber/VRScrollAdapter.hpp"

#include <UnityEngine/EventSystems/EventSystem.hpp>
#include <UnityEngine/EventSystems/PointerEventData.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Vector2.hpp>
#include <Zenject/DiContainer.hpp>

#include "reactive/beatsaber/BeatSaberUtils.hpp"

DEFINE_TYPE(Reactive::BeatSaber, VRScrollAdapter);

namespace Reactive::BeatSaber {
namespace {
    // The game's lists: the sqrMagnitude check in HMUI.ScrollView.CheckScrollInput
    constexpr float kDeadzoneSqr = 0.01f;
}

void VRScrollAdapter::Awake() {
    _pointerEventsHandler = get_gameObject()->GetComponent<Reactive::PointerEventsHandler*>();

    // Null before the first menu loads.
    if (auto* container = ::reactive::beatsaber::g_menuContainer) {
        _platformHelper = reinterpret_cast<::GlobalNamespace::IVRPlatformHelper*>(
            container->Resolve(csTypeOf(::GlobalNamespace::IVRPlatformHelper*)));
    }
}

void VRScrollAdapter::Update() {
    if (!_platformHelper || !_pointerEventsHandler->IsHovered) return;

    auto delta = _platformHelper->GetAnyJoystickMaxAxis();
    if (delta.x * delta.x + delta.y * delta.y <= kDeadzoneSqr) return;

    auto* eventData = UnityEngine::EventSystems::PointerEventData::New_ctor(UnityEngine::EventSystems::EventSystem::get_current());
    eventData->set_scrollDelta(delta);
    _pointerEventsHandler->OnScroll(eventData);
}
}
