#include "reactive/beatsaber/Spinner.hpp"

#include <UnityEngine/Time.hpp>
#include <UnityEngine/Vector3.hpp>

#include "reactive/beatsaber/BeatSaberResources.hpp"

namespace reactive::beatsaber {
UnityEngine::GameObject* Spinner::Construct() {
    // The bundle loads asynchronously at boot, so the default sprite is resolved as late as possible.
    if (!_image->GetSprite()) {
        if (auto* s = Sprites()) _image->SetSprite(s->spinnerIcon);
    }
    return _image->Use(nullptr);
}

void Spinner::OnUpdate() {
    auto rect = ContentTransform();
    auto e = rect->get_localEulerAngles();
    const float dz = UnityEngine::Time::get_deltaTime() * RotationSpeed * 10.f;
    rect->set_localEulerAngles({e.x, e.y, e.z - dz});
}
}
