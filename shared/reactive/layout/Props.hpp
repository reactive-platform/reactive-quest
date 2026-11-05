#pragma once

// Every component takes one aggregate Props struct, so designated initializers stand in for PC's object initializers, and REACTIVE_PROP for its StateGenerator.
// Clang accepts designators in any order and only warns, and qpm.json passes -Wno-reorder-init-list to consumers.

#include <optional>
#include <string>
#include <vector>

#include "reactive/state/State.hpp"
#include "reactive/yoga/Enums.hpp"
#include "reactive/yoga/Types.hpp"

namespace reactive {
class IReactiveComponent;

// Declares a bindable prop: `name` takes a value and `stateName` a state binding. With both, the value is applied first.
#define REACTIVE_PROP(Type, name, stateName) \
    std::optional<Type> name{};              \
    ::reactive::StateBinding<Type> stateName {}

struct FlexItemProps {
    std::optional<float>        flex;
    std::optional<float>        flexGrow;
    std::optional<float>        flexShrink;
    std::optional<YogaValue>    flexBasis;
    YogaVector                  size{};
    YogaVector                  minSize{};
    YogaVector                  maxSize{};
    YogaFrame                   margin{};
    YogaFrame                   position{};
    std::optional<float>        aspectRatio;
    std::optional<PositionType> positionType;
    std::optional<Align>        alignSelf;

    StateBinding<float>      sFlex;
    StateBinding<float>      sFlexGrow;
    StateBinding<float>      sFlexShrink;
    StateBinding<YogaValue>  sFlexBasis;
    StateBinding<YogaVector> sSize;
    StateBinding<YogaVector> sMinSize;
    StateBinding<YogaVector> sMaxSize;
    StateBinding<YogaFrame>  sMargin;
    StateBinding<YogaFrame>  sPosition;
};

struct FlexControllerProps {
    std::optional<FlexDirection> flexDirection;
    std::optional<Justify>       justifyContent;
    std::optional<Align>         alignItems;
    std::optional<Align>         alignContent;
    std::optional<Wrap>          flexWrap;
    std::optional<Overflow>      overflow;
    std::optional<Direction>     direction;
    YogaFrame                    padding{};
    YogaVector                   gap{};
    std::optional<bool>          constrainHorizontal;
    std::optional<bool>          constrainVertical;
};

#define REACTIVE_ITEM_PROPS                                \
    ::reactive::FlexItemProps flexItem{};                  \
    REACTIVE_PROP(bool, enabled, sEnabled);                \
    std::optional<bool> withinLayoutIfDisabled{};          \
    std::optional<std::string> name {}

#define REACTIVE_LAYOUT_PROPS                              \
    ::reactive::FlexControllerProps flexController{};      \
    REACTIVE_ITEM_PROPS;                                   \
    std::vector<::reactive::IReactiveComponent*> children {}

struct ItemProps {
    REACTIVE_ITEM_PROPS;
};
}
