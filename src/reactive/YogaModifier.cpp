#include "reactive/yoga/YogaModifier.hpp"

#include "reactive/YogaStyle.hpp"

namespace reactive {
void YogaModifier::Attach(YGNodeRef hostNode, std::function<void()> onUpdated) {
    _node      = hostNode;
    _onUpdated = std::move(onUpdated);
}

#define SETTER(name, type, action)        \
    void YogaModifier::Set##name(type v) { \
        action;                            \
        _onUpdated();                      \
    }

SETTER(PositionType, PositionType, YGNodeStyleSetPositionType(_node, static_cast<YGPositionType>(v)))
SETTER(AlignSelf,    Align,        YGNodeStyleSetAlignSelf(_node, static_cast<YGAlign>(v)))
SETTER(Flex,         float,        YGNodeStyleSetFlex(_node, v))
SETTER(FlexGrow,     float,        YGNodeStyleSetFlexGrow(_node, v))
SETTER(FlexShrink,   float,        YGNodeStyleSetFlexShrink(_node, v))
SETTER(AspectRatio,  float,        YGNodeStyleSetAspectRatio(_node, v))
SETTER(FlexBasis,    YogaValue,    yogastyle::FlexBasis(_node, v))
SETTER(Position,     YogaFrame,    yogastyle::Position(_node, v))
SETTER(Margin,       YogaFrame,    yogastyle::Margin(_node, v))
SETTER(Size,         YogaVector,   (yogastyle::Width(_node, v.x), yogastyle::Height(_node, v.y)))
SETTER(MinSize,      YogaVector,   (yogastyle::MinWidth(_node, v.x), yogastyle::MinHeight(_node, v.y)))
SETTER(MaxSize,      YogaVector,   (yogastyle::MaxWidth(_node, v.x), yogastyle::MaxHeight(_node, v.y)))

#undef SETTER

void YogaModifier::Apply(FlexItemProps const& p) {
    if (p.positionType)            SetPositionType(*p.positionType);
    if (p.alignSelf)               SetAlignSelf(*p.alignSelf);
    if (p.flex)                    SetFlex(*p.flex);
    if (p.flexGrow)                SetFlexGrow(*p.flexGrow);
    if (p.flexShrink)              SetFlexShrink(*p.flexShrink);
    if (p.flexBasis)               SetFlexBasis(*p.flexBasis);
    if (!p.size.IsUndefined())     SetSize(p.size);
    if (!p.minSize.IsUndefined())  SetMinSize(p.minSize);
    if (!p.maxSize.IsUndefined())  SetMaxSize(p.maxSize);
    if (!p.position.IsUndefined()) SetPosition(p.position);
    if (!p.margin.IsUndefined())   SetMargin(p.margin);
    if (p.aspectRatio)             SetAspectRatio(*p.aspectRatio);
}
}
