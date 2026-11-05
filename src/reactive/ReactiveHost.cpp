#include "reactive/ReactiveHost.hpp"
#include "reactive/HostState.hpp"

DEFINE_TYPE(Reactive, ReactiveHost);

namespace {
    reactive::HostState* state_of(Reactive::ReactiveHost* self) {
        return reinterpret_cast<reactive::HostState*>(self->_native);
    }
}

void Reactive::ReactiveHost::Start() {
    if (auto* s = state_of(this)) s->OnStart();
}

void Reactive::ReactiveHost::Update() {
    if (auto* s = state_of(this)) s->OnUpdate();
}

void Reactive::ReactiveHost::OnEnable() {
    if (auto* s = state_of(this)) s->OnEnable();
}

void Reactive::ReactiveHost::OnDisable() {
    if (auto* s = state_of(this)) s->OnDisable();
}

void Reactive::ReactiveHost::OnDestroy() {
    if (auto* s = state_of(this)) {
        s->OnDestroy();
        delete s;
        this->_native = 0;
    }
}

void Reactive::ReactiveHost::OnRectTransformDimensionsChange() {
    if (auto* s = state_of(this)) s->OnRectTransformDimensionsChange();
}
