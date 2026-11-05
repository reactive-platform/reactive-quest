#include "reactive/beatsaber/RaycastMask2D.hpp"

#include <UnityEngine/Vector3.hpp>

DEFINE_TYPE(Reactive::BeatSaber, RaycastMask2D);

void Reactive::BeatSaber::RaycastMask2D::Awake() {
    _rectTransform = this->GetComponent<UnityEngine::RectTransform*>();
}

bool Reactive::BeatSaber::RaycastMask2D::ValidatePoint(UnityEngine::Vector2 point) {
    UnityEngine::Vector3 v3{point.x, point.y, 0.f};
    auto local = _rectTransform->InverseTransformPoint(v3);
    return ValidateLocalPoint({local.x, local.y});
}

bool Reactive::BeatSaber::RaycastMask2D::ValidateLocalPoint(UnityEngine::Vector2 point) {
    if (!this->get_isActiveAndEnabled()) return true;
    auto rect = _rectTransform->get_rect();
    // Rect::Contains(Vector2) isn't always linked.
    return point.x >= rect.m_XMin && point.x <= rect.m_XMin + rect.m_Width &&
           point.y >= rect.m_YMin && point.y <= rect.m_YMin + rect.m_Height;
}
