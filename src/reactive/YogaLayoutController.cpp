#include "reactive/yoga/YogaLayoutController.hpp"

#include <exception>
#include <limits>

#include "main.hpp"
#include "reactive/HostState.hpp"
#include "reactive/YogaStyle.hpp"

namespace reactive {
namespace {
    YGSize MeasureLeaf(YGNodeConstRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        auto* leaf = static_cast<ILeafLayoutItem*>(YGNodeGetContext(const_cast<YGNodeRef>(node)));
        // An exception must not unwind through Yoga.
        try {
            auto size = leaf->Measure(width, static_cast<MeasureMode>(widthMode), height, static_cast<MeasureMode>(heightMode));
            return YGSize{size.x, size.y};
        } catch (std::exception const& e) {
            ReactiveLogger.error("Reactive: measuring threw: {}", e.what());
        } catch (...) {
            ReactiveLogger.error("Reactive: measuring threw an unknown exception");
        }
        return YGSize{0.f, 0.f};
    }
}

void YogaLayoutController::Attach(YGNodeRef hostNode, std::function<void()> onUpdated) {
    _node      = hostNode;
    _onUpdated = std::move(onUpdated);
    ApplyAll();
}

void YogaLayoutController::InsertChild(IReactiveComponent* item, int index) {
    auto* node = item->GetHostState()->GetYogaNode();
    YGNodeInsertChild(_node, node, index);

    if (auto* leaf = dynamic_cast<ILeafLayoutItem*>(item)) {
        YGNodeSetContext(node, leaf);
        YGNodeSetMeasureFunc(node, &MeasureLeaf);
        YGNodeMarkDirty(node);
    }
}

void YogaLayoutController::RemoveChild(YGNodeRef node) {
    // Yoga doesn't allow measure funcs on parents, and the node may be reused as one.
    YGNodeSetMeasureFunc(node, nullptr);
    YGNodeSetContext(node, nullptr);
    YGNodeRemoveChild(_node, node);
}

LayoutDimensions YogaLayoutController::Recalculate(UnityEngine::Vector2 constraints) {
    if (!_constrainHorizontal) constraints.x = std::numeric_limits<float>::quiet_NaN();
    if (!_constrainVertical)   constraints.y = std::numeric_limits<float>::quiet_NaN();

    YGNodeCalculateLayout(_node, constraints.x, constraints.y,
                          static_cast<YGDirection>(_direction.value_or(Direction::LeftToRight)));
    return Dimensions(_node);
}

LayoutDimensions YogaLayoutController::Dimensions(YGNodeRef node) {
    return {YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node), YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node)};
}

void YogaLayoutController::ApplyAll() {
    if (_flexDirection)  YGNodeStyleSetFlexDirection(_node, static_cast<YGFlexDirection>(*_flexDirection));
    if (_overflow)       YGNodeStyleSetOverflow(_node, static_cast<YGOverflow>(*_overflow));
    if (_direction)      YGNodeStyleSetDirection(_node, static_cast<YGDirection>(*_direction));
    if (_wrap)           YGNodeStyleSetFlexWrap(_node, static_cast<YGWrap>(*_wrap));
    if (_justifyContent) YGNodeStyleSetJustifyContent(_node, static_cast<YGJustify>(*_justifyContent));
    if (_alignItems)     YGNodeStyleSetAlignItems(_node, static_cast<YGAlign>(*_alignItems));
    if (_alignContent)   YGNodeStyleSetAlignContent(_node, static_cast<YGAlign>(*_alignContent));
    if (_padding)        yogastyle::Padding(_node, *_padding);
    if (_gap)            yogastyle::Gap(_node, *_gap);
}

#define CSETTER(name, type, field, action) \
    void YogaLayoutController::Set##name(type v) { \
        field = v;                                 \
        if (_node) {                               \
            action;                                \
            _onUpdated();                          \
        }                                          \
    }

CSETTER(Overflow,       Overflow,      _overflow,       YGNodeStyleSetOverflow(_node, static_cast<YGOverflow>(v)))
CSETTER(Direction,      Direction,     _direction,      YGNodeStyleSetDirection(_node, static_cast<YGDirection>(v)))
CSETTER(FlexDirection,  FlexDirection, _flexDirection,  YGNodeStyleSetFlexDirection(_node, static_cast<YGFlexDirection>(v)))
CSETTER(FlexWrap,       Wrap,          _wrap,           YGNodeStyleSetFlexWrap(_node, static_cast<YGWrap>(v)))
CSETTER(JustifyContent, Justify,       _justifyContent, YGNodeStyleSetJustifyContent(_node, static_cast<YGJustify>(v)))
CSETTER(AlignItems,     Align,         _alignItems,     YGNodeStyleSetAlignItems(_node, static_cast<YGAlign>(v)))
CSETTER(AlignContent,   Align,         _alignContent,   YGNodeStyleSetAlignContent(_node, static_cast<YGAlign>(v)))
CSETTER(Padding,        YogaFrame,     _padding,        yogastyle::Padding(_node, v))
CSETTER(Gap,            YogaVector,    _gap,            yogastyle::Gap(_node, v))

void YogaLayoutController::Apply(FlexControllerProps const& p) {
    if (p.flexDirection)          SetFlexDirection(*p.flexDirection);
    if (p.justifyContent)         SetJustifyContent(*p.justifyContent);
    if (p.alignItems)             SetAlignItems(*p.alignItems);
    if (p.alignContent)           SetAlignContent(*p.alignContent);
    if (p.flexWrap)               SetFlexWrap(*p.flexWrap);
    if (p.overflow)               SetOverflow(*p.overflow);
    if (p.direction)              SetDirection(*p.direction);
    if (!p.padding.IsUndefined()) SetPadding(p.padding);
    if (!p.gap.IsUndefined())     SetGap(p.gap);
    if (p.constrainHorizontal)    SetConstrainHorizontal(*p.constrainHorizontal);
    if (p.constrainVertical)      SetConstrainVertical(*p.constrainVertical);
}
}
