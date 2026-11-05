#pragma once

#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/RectTransform.hpp>
#include <UnityEngine/Transform.hpp>

namespace reactive {
struct HostState;

class IReactiveComponent {
public:
    virtual ~IReactiveComponent() = default;

    virtual UnityEngine::GameObject*    Content()          = 0;
    virtual UnityEngine::RectTransform* ContentTransform() = 0;

    virtual bool IsInitialized() const = 0;
    virtual bool GetEnabled()    const = 0;
    virtual void SetEnabled(bool v)    = 0;

    virtual UnityEngine::GameObject* Use(UnityEngine::Transform* parent) = 0;

    virtual HostState* GetHostState() const = 0;

    virtual void RecalculateLayoutImmediate()  = 0;
    virtual void ScheduleLayoutRecalculation() = 0;
};
}
