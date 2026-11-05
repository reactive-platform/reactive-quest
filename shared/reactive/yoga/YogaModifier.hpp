#pragma once

#include <functional>
#include <optional>

#include <yoga/Yoga.h>

#include "reactive/layout/Props.hpp"
#include "reactive/yoga/Enums.hpp"
#include "reactive/yoga/Types.hpp"

namespace reactive {
class YogaModifier {
public:
    void Attach(YGNodeRef hostNode, std::function<void()> onUpdated);

    void SetPositionType(PositionType v);
    void SetAlignSelf(Align v);
    void SetFlex(float v);
    void SetFlexGrow(float v);
    void SetFlexShrink(float v);
    void SetFlexBasis(YogaValue v);
    void SetPosition(YogaFrame v);
    void SetSize(YogaVector v);
    void SetMinSize(YogaVector v);
    void SetMaxSize(YogaVector v);
    void SetAspectRatio(float v);
    void SetMargin(YogaFrame v);

    // State-bound fields need a lifetime owner, so ReactiveComponent::ApplyFlexItem binds those.
    void Apply(FlexItemProps const& props);

private:
    YGNodeRef             _node{nullptr};
    std::function<void()> _onUpdated;
};
}
