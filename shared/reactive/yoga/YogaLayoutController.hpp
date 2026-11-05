#pragma once

#include <functional>
#include <optional>

#include <yoga/Yoga.h>

#include "reactive/IReactiveComponent.hpp"
#include "reactive/layout/Interfaces.hpp"
#include "reactive/layout/Props.hpp"
#include "reactive/yoga/Enums.hpp"
#include "reactive/yoga/Types.hpp"

namespace reactive {
class YogaLayoutController {
public:
    void Attach(YGNodeRef hostNode, std::function<void()> onUpdated);

    void InsertChild(IReactiveComponent* item, int index);
    void RemoveChild(YGNodeRef node);

    LayoutDimensions Recalculate(UnityEngine::Vector2 constraints);

    static LayoutDimensions Dimensions(YGNodeRef node);

    void SetOverflow(Overflow v);
    void SetDirection(Direction v);
    void SetFlexDirection(FlexDirection v);
    void SetFlexWrap(Wrap v);
    void SetJustifyContent(Justify v);
    void SetAlignItems(Align v);
    void SetAlignContent(Align v);
    void SetPadding(YogaFrame v);
    void SetGap(YogaVector v);
    void SetConstrainHorizontal(bool v) { _constrainHorizontal = v; }
    void SetConstrainVertical(bool v)   { _constrainVertical = v; }

    void Apply(FlexControllerProps const& props);

private:
    YGNodeRef             _node{nullptr};
    std::function<void()> _onUpdated;

    std::optional<Overflow>      _overflow;
    std::optional<Direction>     _direction;
    std::optional<FlexDirection> _flexDirection{FlexDirection::Row};
    std::optional<Wrap>          _wrap;
    std::optional<Justify>       _justifyContent;
    std::optional<Align>         _alignItems;
    std::optional<Align>         _alignContent;
    std::optional<YogaFrame>     _padding;
    std::optional<YogaVector>    _gap;
    bool _constrainHorizontal{true};
    bool _constrainVertical{true};

    void ApplyAll();
};
}
