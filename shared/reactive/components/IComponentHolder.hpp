#pragma once

namespace reactive {
// HolderTag is a different type per T, so a class can implement GetHeldComponent for several T without a covariance collision.
template <class T>
class IComponentHolder {
public:
    virtual ~IComponentHolder() = default;

    struct HolderTag {};

    virtual T* GetHeldComponent(HolderTag = {}) = 0;
};

template <class T, class Self>
T* Held(Self* s) {
    return static_cast<IComponentHolder<T>*>(s)->GetHeldComponent();
}
}
