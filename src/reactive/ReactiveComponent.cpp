#include "reactive/ReactiveComponent.hpp"
#include "reactive/HostState.hpp"
#include "reactive/ReactiveHost.hpp"

#include <cxxabi.h>
#include <cstdlib>
#include <typeinfo>

#include <UnityEngine/Object.hpp>

#include "reactive/layout/Layout.hpp"
#include "reactive/yoga/YogaModifier.hpp"

namespace reactive {
namespace {
    std::string ShortTypeName(std::type_info const& info) {
        int   status    = 0;
        char* demangled = abi::__cxa_demangle(info.name(), nullptr, nullptr, &status);
        std::string name = (status == 0 && demangled) ? demangled : info.name();
        std::free(demangled);

        if (auto lt = name.find('<'); lt != std::string::npos) name.resize(lt);
        if (auto sep = name.rfind("::"); sep != std::string::npos) name = name.substr(sep + 2);
        return name;
    }

    bool HasAnyValue(FlexItemProps const& p) {
        return p.flex || p.flexGrow || p.flexShrink || p.flexBasis || !p.size.IsUndefined() ||
               !p.minSize.IsUndefined() || !p.maxSize.IsUndefined() || !p.margin.IsUndefined() ||
               !p.position.IsUndefined() || p.aspectRatio || p.positionType || p.alignSelf ||
               p.sFlex || p.sFlexGrow || p.sFlexShrink || p.sFlexBasis || p.sSize || p.sMinSize ||
               p.sMaxSize || p.sMargin || p.sPosition;
    }
}

ReactiveComponent::ReactiveComponent() = default;

ReactiveComponent::~ReactiveComponent() {
    for (auto& binding : _bindings) binding.subscription.RemoveCallback();
    for (auto& state : _ownedStates) state->ClearCallbacks();
}

UnityEngine::GameObject* ReactiveComponent::Content() {
    EnsureInitialized();
    return _content;
}

UnityEngine::RectTransform* ReactiveComponent::ContentTransform() {
    EnsureInitialized();
    return _contentTransform;
}

bool ReactiveComponent::GetEnabled() const {
    if (!_isInitialized) return _pendingEnabled.value_or(true);
    return _content->get_activeSelf();
}

void ReactiveComponent::SetEnabled(bool v) {
    if (!_isInitialized) {
        _pendingEnabled = v;
        return;
    }
    _host->SetEnabled(v);
}

void ReactiveComponent::SetName(std::string name) {
    if (!_isInitialized) {
        _pendingName = std::move(name);
        return;
    }
    _content->set_name(StringW(name));
}

void ReactiveComponent::SetWithinLayoutIfDisabled(bool v) {
    if (!_host) {
        _pendingWithinLayoutIfDisabled = v;
        return;
    }
    _host->SetWithinLayoutIfDisabled(v);
}

UnityEngine::GameObject* ReactiveComponent::Use(UnityEngine::Transform* parent) {
    EnsureInitialized();
    if (auto* driver = _host->GetLayoutDriver()) driver->DetachChild(_host);
    _contentTransform->SetParent(parent, false);
    return _content;
}

void ReactiveComponent::RecalculateLayoutImmediate() {
    if (_host) _host->RecalculateLayoutImmediate();
}

void ReactiveComponent::ScheduleLayoutRecalculation() {
    if (_host) _host->ScheduleLayoutRecalculation();
}

void ReactiveComponent::NotifyLayoutDataUpdated() {
    if (_host) _host->NotifyLayoutDataUpdated();
}

YogaModifier& ReactiveComponent::FlexItem() {
    EnsureInitialized();
    return _host->GetModifier();
}

void ReactiveComponent::ApplyFlexItem(FlexItemProps const& p) {
    if (!HasAnyValue(p)) return;
    if (!_host) {
        _pendingFlexItems.push_back(p);
        return;
    }

    auto* m = &_host->GetModifier();
    m->Apply(p);

    if (p.sFlex)       BindState<float>(p.sFlex.state, [m](float const& v) { m->SetFlex(v); }, p.sFlex.lazy);
    if (p.sFlexGrow)   BindState<float>(p.sFlexGrow.state, [m](float const& v) { m->SetFlexGrow(v); }, p.sFlexGrow.lazy);
    if (p.sFlexShrink) BindState<float>(p.sFlexShrink.state, [m](float const& v) { m->SetFlexShrink(v); }, p.sFlexShrink.lazy);
    if (p.sFlexBasis)  BindState<YogaValue>(p.sFlexBasis.state, [m](YogaValue const& v) { m->SetFlexBasis(v); }, p.sFlexBasis.lazy);
    if (p.sSize)       BindState<YogaVector>(p.sSize.state, [m](YogaVector const& v) { m->SetSize(v); }, p.sSize.lazy);
    if (p.sMinSize)    BindState<YogaVector>(p.sMinSize.state, [m](YogaVector const& v) { m->SetMinSize(v); }, p.sMinSize.lazy);
    if (p.sMaxSize)    BindState<YogaVector>(p.sMaxSize.state, [m](YogaVector const& v) { m->SetMaxSize(v); }, p.sMaxSize.lazy);
    if (p.sMargin)     BindState<YogaFrame>(p.sMargin.state, [m](YogaFrame const& v) { m->SetMargin(v); }, p.sMargin.lazy);
    if (p.sPosition)   BindState<YogaFrame>(p.sPosition.state, [m](YogaFrame const& v) { m->SetPosition(v); }, p.sPosition.lazy);
}

void ReactiveComponent::StartCoroutine(custom_types::Helpers::Coroutine coroutine) {
    EnsureInitialized();
    _host->monoBehaviour->::UnityEngine::MonoBehaviour::StartCoroutine(
        custom_types::Helpers::CoroutineHelper::New(std::move(coroutine)));
}

void ReactiveComponent::StopAllCoroutines() {
    if (_host) _host->monoBehaviour->::UnityEngine::MonoBehaviour::StopAllCoroutines();
}

void ReactiveComponent::UpdateModules() {
    for (size_t i = 0; i < _modules.size(); ++i) _modules[i]->OnUpdate();
}

void ReactiveComponent::DispatchLayoutApplied() {
    if (_layoutAppliedListeners.empty()) return;
    auto snapshot = _layoutAppliedListeners;
    for (auto& cb : snapshot) cb(*this);
}

UnityEngine::GameObject* ReactiveComponent::Construct() {
    auto* go   = UnityEngine::GameObject::New_ctor();
    auto* rect = go->AddComponent<UnityEngine::RectTransform*>();
    this->Construct(rect);
    return go;
}

void ReactiveComponent::EnsureInitialized() {
    if (!_isInitialized && !_isConstructing) ConstructAndInit();
}

void ReactiveComponent::ConstructAndInit() {
    _isConstructing = true;
    _content = Construct();
    _isConstructing = false;

    _contentTransform = _content->GetComponent<UnityEngine::RectTransform*>();
    if (!_contentTransform) {
        _contentTransform = _content->AddComponent<UnityEngine::RectTransform*>();
    }
    _content->set_name(StringW(_pendingName ? *_pendingName : ShortTypeName(typeid(*this))));
    _pendingName.reset();

    // One host per GameObject, so wrapper components share the same Yoga node and modifier.
    Reactive::ReactiveHost* mb = _content->GetComponent<Reactive::ReactiveHost*>();
    if (mb) {
        _host = reinterpret_cast<HostState*>(mb->_native);
    } else {
        mb    = _content->AddComponent<Reactive::ReactiveHost*>();
        _host = new HostState(mb, _contentTransform);
        mb->_native = reinterpret_cast<int64_t>(_host);
    }

    _isInitialized = true;

    _host->AddComponent(std::unique_ptr<ReactiveComponent>(this));

    auto flexItems = std::move(_pendingFlexItems);
    _pendingFlexItems.clear();
    for (auto const& item : flexItems) ApplyFlexItem(item);

    if (_pendingWithinLayoutIfDisabled) _host->SetWithinLayoutIfDisabled(*_pendingWithinLayoutIfDisabled);

    OnInitialize();

    // Applied last, like PC's initializer after the constructor, so OnDisable sees an initialized component.
    if (_pendingEnabled) _host->SetEnabled(*_pendingEnabled);
}
}
