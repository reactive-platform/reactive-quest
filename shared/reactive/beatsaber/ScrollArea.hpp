#pragma once

#include "reactive/components/ScrollArea.hpp"

namespace reactive::beatsaber {
using ::reactive::components::ScrollContext;
using ::reactive::components::ScrollOrientation;

class ScrollArea : public ::reactive::components::ScrollArea {
public:
    using ::reactive::components::ScrollArea::ScrollArea;

protected:
    void OnInitialize() override;
};
}
