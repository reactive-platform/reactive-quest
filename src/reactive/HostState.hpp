#pragma once

#include <memory>
#include <vector>

#include <UnityEngine/RectTransform.hpp>
#include <yoga/Yoga.h>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/layout/Interfaces.hpp"
#include "reactive/yoga/YogaModifier.hpp"

namespace Reactive {
    class ReactiveHost;
}

namespace reactive {
class Layout;

// One per GameObject. Owns the ReactiveComponents attached to it.
struct HostState {
    HostState(Reactive::ReactiveHost* host, UnityEngine::RectTransform* rect);
    ~HostState();

    void AddComponent(std::unique_ptr<ReactiveComponent> component);

    Reactive::ReactiveHost* monoBehaviour;

    void OnStart();
    void OnUpdate();
    void OnEnable();
    void OnDisable();
    void OnDestroy();
    void OnRectTransformDimensionsChange();

    YGNodeRef     GetYogaNode();
    YogaModifier& GetModifier();

    Layout* GetLayoutDriver() const { return _layoutDriver; }
    void    SetLayoutDriver(Layout* driver);

    void SetEnabled(bool enabled);
    void SetWithinLayoutIfDisabled(bool v);

    void ApplyLayout(LayoutDimensions const& dimensions);
    void RecalculateLayoutImmediate();
    void ScheduleLayoutRecalculation();
    void NotifyLayoutDataUpdated();

    HostState* LayoutRoot();
    bool       IsActiveInHierarchy() const;

    bool layoutQueued{false};
    bool layoutParked{false};

private:
    UnityEngine::RectTransform* _rect;

    std::vector<std::unique_ptr<ReactiveComponent>> _components;

    Layout*                       _layoutDriver{nullptr};
    std::unique_ptr<YogaModifier> _modifier;
    YGNodeRef                     _yogaNode{nullptr};

    LayoutDimensions _lastApplied{};
    bool             _hasApplied{false};
    bool             _beingApplied{false};

    bool _isStarted{false};
    bool _wasActuallyDisabled{false};
    bool _withinLayoutIfDisabled{false};

    bool IsControlledByParent() const;
    void ApplyDisplay();
};
}
