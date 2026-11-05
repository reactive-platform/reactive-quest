#pragma once

namespace reactive {
class IReactiveModule {
public:
    virtual ~IReactiveModule() = default;

    virtual void OnUpdate() = 0;
};
}
