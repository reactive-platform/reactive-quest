#pragma once

#include <functional>
#include <vector>

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/EventSystems/IPointerDownHandler.hpp"
#include "UnityEngine/EventSystems/IPointerUpHandler.hpp"
#include "UnityEngine/EventSystems/IPointerEnterHandler.hpp"
#include "UnityEngine/EventSystems/IPointerExitHandler.hpp"
#include "UnityEngine/EventSystems/IDragHandler.hpp"
#include "UnityEngine/EventSystems/IBeginDragHandler.hpp"
#include "UnityEngine/EventSystems/IEndDragHandler.hpp"
#include "UnityEngine/EventSystems/IScrollHandler.hpp"
#include "UnityEngine/EventSystems/PointerEventData.hpp"

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(
    Reactive, PointerEventsHandler,
    UnityEngine::MonoBehaviour,
    UnityEngine::EventSystems::IPointerDownHandler*,
    UnityEngine::EventSystems::IPointerUpHandler*,
    UnityEngine::EventSystems::IPointerEnterHandler*,
    UnityEngine::EventSystems::IPointerExitHandler*,
    UnityEngine::EventSystems::IDragHandler*,
    UnityEngine::EventSystems::IBeginDragHandler*,
    UnityEngine::EventSystems::IEndDragHandler*,
    UnityEngine::EventSystems::IScrollHandler*
) {
    DECLARE_INSTANCE_FIELD(bool, IsPressed);
    DECLARE_INSTANCE_FIELD(bool, IsHovered);
    DECLARE_INSTANCE_FIELD(bool, IsDragging);
    DECLARE_INSTANCE_FIELD(UnityEngine::EventSystems::PointerEventData*, EventData);

    DECLARE_CTOR(ctor);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);

    DECLARE_OVERRIDE_METHOD(void, OnPointerDown,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IPointerDownHandler::OnPointerDown>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);
    DECLARE_OVERRIDE_METHOD(void, OnPointerUp,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IPointerUpHandler::OnPointerUp>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);
    DECLARE_OVERRIDE_METHOD(void, OnPointerEnter,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IPointerEnterHandler::OnPointerEnter>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);
    DECLARE_OVERRIDE_METHOD(void, OnPointerExit,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IPointerExitHandler::OnPointerExit>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);
    DECLARE_OVERRIDE_METHOD(void, OnDrag,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IDragHandler::OnDrag>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);
    DECLARE_OVERRIDE_METHOD(void, OnBeginDrag,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IBeginDragHandler::OnBeginDrag>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);
    DECLARE_OVERRIDE_METHOD(void, OnEndDrag,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IEndDragHandler::OnEndDrag>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);
    DECLARE_OVERRIDE_METHOD(void, OnScroll,
        il2cpp_utils::il2cpp_type_check::MetadataGetter<&UnityEngine::EventSystems::IScrollHandler::OnScroll>::methodInfo(),
        UnityEngine::EventSystems::PointerEventData*);

public:
    using PointerCallback = std::function<void(Reactive::PointerEventsHandler*, UnityEngine::EventSystems::PointerEventData*)>;

    void AddPointerDownListener(PointerCallback cb)  { _listeners.down.push_back(std::move(cb)); }
    void AddPointerUpListener(PointerCallback cb)    { _listeners.up.push_back(std::move(cb)); }
    void AddPointerEnterListener(PointerCallback cb) { _listeners.enter.push_back(std::move(cb)); }
    void AddPointerExitListener(PointerCallback cb)  { _listeners.exit.push_back(std::move(cb)); }
    void AddDragListener(PointerCallback cb)         { _listeners.drag.push_back(std::move(cb)); }
    void AddBeginDragListener(PointerCallback cb)    { _listeners.beginDrag.push_back(std::move(cb)); }
    void AddEndDragListener(PointerCallback cb)      { _listeners.endDrag.push_back(std::move(cb)); }
    void AddScrollListener(PointerCallback cb)       { _listeners.scroll.push_back(std::move(cb)); }

    bool IsFocused() const { return IsDragging || IsPressed || IsHovered; }

private:
    struct Listeners {
        std::vector<PointerCallback> down, up, enter, exit, drag, beginDrag, endDrag, scroll;
    };
    Listeners _listeners;
};
