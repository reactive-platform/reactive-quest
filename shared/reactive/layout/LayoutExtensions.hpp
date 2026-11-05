#pragma once

// Translation units that chain AsFlexItem or AsFlexGroup must include this header.

#include "reactive/ReactiveComponent.hpp"
#include "reactive/layout/Layout.hpp"
#include "reactive/layout/Props.hpp"
#include "reactive/yoga/YogaLayoutController.hpp"
#include "reactive/yoga/YogaModifier.hpp"

namespace reactive {
template <class Self>
Self* ReactiveComponent::AsFlexItem(this Self& self, FlexItemProps const& props) {
    self.ApplyFlexItem(props);
    return &self;
}

template <class Self>
Self* Layout::AsFlexGroup(this Self& self, FlexControllerProps const& props) {
    self.FlexController().Apply(props);
    return &self;
}
}
