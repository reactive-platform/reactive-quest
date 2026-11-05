#include "reactive/layout/Layout.hpp"

#include <algorithm>
#include <limits>

#include <UnityEngine/Vector2.hpp>

#include "reactive/HostState.hpp"

namespace reactive {
namespace {
    bool HasAnyValue(FlexControllerProps const& p) {
        return p.flexDirection || p.justifyContent || p.alignItems || p.alignContent || p.flexWrap ||
               p.overflow || p.direction || !p.padding.IsUndefined() || !p.gap.IsUndefined() ||
               p.constrainHorizontal || p.constrainVertical;
    }
}

Layout::~Layout() {
    for (auto* child : _children) child->GetHostState()->SetLayoutDriver(nullptr);
}

void Layout::ApplyLayoutProps(FlexControllerProps const& controller, std::vector<IReactiveComponent*> const& children) {
    if (HasAnyValue(controller)) FlexController().Apply(controller);
    for (auto* c : children) AddChild(c);
}

YogaLayoutController& Layout::FlexController() {
    if (!_controller) {
        _controller = std::make_unique<YogaLayoutController>();
        if (IsInitialized()) AttachController();
    }
    return *_controller;
}

void Layout::AttachController() {
    if (!_controller) return;
    _controller->Attach(GetHostState()->GetYogaNode(), [this] { ScheduleLayoutRecalculation(); });

    int index = 0;
    for (auto* child : _children) _controller->InsertChild(child, index++);
    ScheduleLayoutRecalculation();
}

void Layout::OnInitialize() {
    AttachController();

    // Runs here because AddChild needs the content.
    auto initial = std::move(_initialChildren);
    _initialChildren.clear();
    for (auto* c : initial) AddChild(c);
}

void Layout::AddChild(IReactiveComponent* item) {
    if (!item) return;
    if (!IsInitialized()) {
        _initialChildren.push_back(item);
        return;
    }

    item->Use(ContentTransform());
    item->GetHostState()->SetLayoutDriver(this);
    _children.push_back(item);

    if (_controller) _controller->InsertChild(item, static_cast<int>(_children.size()) - 1);
    ScheduleLayoutRecalculation();
}

void Layout::RemoveChild(IReactiveComponent* item) {
    if (!IsInitialized()) {
        std::erase(_initialChildren, item);
        return;
    }
    if (item->GetHostState()->GetLayoutDriver() == this) item->Use(nullptr);
}

void Layout::ClearChildren() {
    if (!IsInitialized()) {
        _initialChildren.clear();
        return;
    }
    while (!_children.empty()) RemoveChild(_children.back());
}

void Layout::DetachChild(HostState* host) {
    auto it = std::find_if(_children.begin(), _children.end(), [host](auto* c) { return c->GetHostState() == host; });
    if (it == _children.end()) return;

    if (_controller) _controller->RemoveChild(host->GetYogaNode());
    host->SetLayoutDriver(nullptr);
    _children.erase(it);
    ScheduleLayoutRecalculation();
}

void Layout::OnRecalculateLayoutSelf() {
    if (!_controller || _children.empty()) return;

    UnityEngine::Vector2 constraints{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};
    if (auto parent = ContentTransform()->get_parent()) {
        if (auto rect = parent->GetComponent<UnityEngine::RectTransform*>()) {
            // Rect::get_size() isn't linked in this build.
            auto r      = rect->get_rect();
            constraints = UnityEngine::Vector2{r.m_Width, r.m_Height};
        }
    }

    GetHostState()->ApplyLayout(_controller->Recalculate(constraints));
}

void Layout::OnLayoutApply() {
    if (!_controller) return;
    // Indexed, because an apply callback may change the children.
    for (size_t i = 0; i < _children.size(); ++i) {
        auto* host = _children[i]->GetHostState();
        auto* node = host->GetYogaNode();
        if (YGNodeStyleGetDisplay(node) == YGDisplayNone) continue;
        host->ApplyLayout(YogaLayoutController::Dimensions(node));
    }
}
}
