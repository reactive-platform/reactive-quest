#include "reactive/HostState.hpp"
#include "reactive/LayoutQueue.hpp"
#include "reactive/ReactiveHost.hpp"

#include <UnityEngine/GameObject.hpp>

#include "reactive/layout/Layout.hpp"

namespace reactive {
namespace {
    // Without this Yoga rounds every size and position to whole canvas units.
    YGConfigRef LayoutConfig() {
        static YGConfigRef config = [] {
            auto* created = YGConfigNew();
            YGConfigSetPointScaleFactor(created, 0.f);
            return created;
        }();
        return config;
    }
}

HostState::HostState(Reactive::ReactiveHost* host, UnityEngine::RectTransform* rect) : monoBehaviour(host), _rect(rect) {}

HostState::~HostState() {
    layoutqueue::Remove(this);
    // Not recursive: child hosts own their nodes, and free them whichever is destroyed first.
    if (_yogaNode) YGNodeFree(_yogaNode);
}

void HostState::AddComponent(std::unique_ptr<ReactiveComponent> component) {
    _components.push_back(std::move(component));
    if (_isStarted) _components.back()->OnStart();
}

YGNodeRef HostState::GetYogaNode() {
    if (!_yogaNode) {
        _yogaNode = YGNodeNewWithConfig(LayoutConfig());
        ApplyDisplay();
    }
    return _yogaNode;
}

YogaModifier& HostState::GetModifier() {
    if (!_modifier) {
        _modifier = std::make_unique<YogaModifier>();
        _modifier->Attach(GetYogaNode(), [this] { ScheduleLayoutRecalculation(); });
        ScheduleLayoutRecalculation();
    }
    return *_modifier;
}

void HostState::OnStart() {
    for (auto& c : _components) c->OnStart();
    _isStarted = true;
}

void HostState::OnUpdate() {
    // Indexed, because a component may be added mid-update.
    for (size_t i = 0; i < _components.size(); ++i) _components[i]->OnUpdate();
    for (size_t i = 0; i < _components.size(); ++i) _components[i]->UpdateModules();
}

void HostState::OnEnable() {
    // Was skipped by the scheduler while inactive.
    if (layoutParked) {
        layoutParked = false;
        ScheduleLayoutRecalculation();
    }

    if (!_wasActuallyDisabled) return;

    ApplyDisplay();
    ScheduleLayoutRecalculation();
    for (auto& c : _components) c->OnEnable();
    _wasActuallyDisabled = false;
}

void HostState::OnDisable() {
    if (monoBehaviour->get_gameObject()->get_activeSelf()) return;

    ApplyDisplay();
    ScheduleLayoutRecalculation();
    for (auto& c : _components) c->OnDisable();
    _wasActuallyDisabled = true;
}

void HostState::OnDestroy() {
    for (auto& c : _components) c->OnDestroyHook();
    if (_layoutDriver) _layoutDriver->DetachChild(this);
    _components.clear();
}

void HostState::OnRectTransformDimensionsChange() {
    // Changes we made ourselves while applying layout don't count.
    if (!_beingApplied) {
        _hasApplied = false;
        ScheduleLayoutRecalculation();
    }
    for (auto& c : _components) c->OnRectDimensionsChanged();
}

void HostState::SetLayoutDriver(Layout* driver) {
    _layoutDriver = driver;
    _hasApplied   = false;
}

void HostState::SetEnabled(bool enabled) {
    auto go = monoBehaviour->get_gameObject();
    go->SetActive(enabled);

    if (!go->get_activeInHierarchy()) {
        // Unity doesn't call OnEnable or OnDisable inside a disabled hierarchy.
        _wasActuallyDisabled = true;
        ApplyDisplay();
        ScheduleLayoutRecalculation();
    }
}

void HostState::SetWithinLayoutIfDisabled(bool v) {
    if (_withinLayoutIfDisabled == v) return;
    _withinLayoutIfDisabled = v;
    ApplyDisplay();
    ScheduleLayoutRecalculation();
}

void HostState::ApplyDisplay() {
    if (!_yogaNode) return;
    const bool active = monoBehaviour->get_gameObject()->get_activeSelf();
    YGNodeStyleSetDisplay(_yogaNode, (active || _withinLayoutIfDisabled) ? YGDisplayFlex : YGDisplayNone);
}

bool HostState::IsControlledByParent() const {
    return _layoutDriver && _layoutDriver->GetLayoutController();
}

void HostState::ApplyLayout(LayoutDimensions const& d) {
    if (!_hasApplied || _lastApplied != d) {
        _beingApplied = true;
        if (IsControlledByParent()) {
            // Keeps the pivot, which rotated children rotate around.
            _rect->SetInsetAndSizeFromParentEdge(UnityEngine::RectTransform::Edge::Top, d.top, d.height);
            _rect->SetInsetAndSizeFromParentEdge(UnityEngine::RectTransform::Edge::Left, d.left, d.width);
        } else {
            // With current anchors, because a raw sizeDelta on a stretched rect would add to the parent's size.
            _rect->SetSizeWithCurrentAnchors(UnityEngine::RectTransform::Axis::Horizontal, d.width);
            _rect->SetSizeWithCurrentAnchors(UnityEngine::RectTransform::Axis::Vertical, d.height);
        }
        _beingApplied = false;
        _lastApplied  = d;
        _hasApplied   = true;
    }

    for (size_t i = 0; i < _components.size(); ++i) _components[i]->OnLayoutApply();
    for (size_t i = 0; i < _components.size(); ++i) _components[i]->DispatchLayoutApplied();
}

HostState* HostState::LayoutRoot() {
    auto* host = this;
    while (host->IsControlledByParent()) host = host->_layoutDriver->GetHostState();
    return host;
}

bool HostState::IsActiveInHierarchy() const {
    return monoBehaviour->get_gameObject()->get_activeInHierarchy();
}

void HostState::RecalculateLayoutImmediate() {
    auto* root = LayoutRoot();
    for (size_t i = 0; i < root->_components.size(); ++i) root->_components[i]->OnRecalculateLayoutSelf();
}

void HostState::ScheduleLayoutRecalculation() {
    if (layoutQueued) return;
    layoutQueued = true;
    layoutqueue::Enqueue(this);
}

void HostState::NotifyLayoutDataUpdated() {
    // Yoga only re-measures a dirty node, and throws when a node without a measure func is marked dirty.
    if (_yogaNode && YGNodeHasMeasureFunc(_yogaNode)) YGNodeMarkDirty(_yogaNode);
    ScheduleLayoutRecalculation();
}
}
