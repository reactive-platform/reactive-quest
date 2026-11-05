#include "main.hpp"

#include <cstring>

#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/register.hpp"

#include <UnityEngine/UI/Image.hpp>

#include <BeatSaber/Init/BSAppInit.hpp>
#include <GlobalNamespace/MainSettingsMenuViewControllersInstaller.hpp>
#include <System/Collections/Generic/List_1.hpp>
#include <UnityEngine/Canvas.hpp>
#include <UnityEngine/Ray.hpp>
#include <UnityEngine/UI/Graphic.hpp>
#include <VRUIControls/VRGraphicRaycaster.hpp>
#include <Zenject/DiContainer.hpp>

#include "reactive/ReactiveScheduler.hpp"
#include "reactive/beatsaber/BeatSaberResources.hpp"
#include "reactive/beatsaber/BeatSaberUtils.hpp"
#include "reactive/beatsaber/FixedImageView.hpp"
#include "reactive/beatsaber/RaycastMask2D.hpp"

namespace reactive::beatsaber {
    Zenject::DiContainer* g_menuContainer = nullptr;
}

MAKE_HOOK_MATCH(MainSettingsInstaller_InstallBindings,
                &GlobalNamespace::MainSettingsMenuViewControllersInstaller::InstallBindings, void,
                GlobalNamespace::MainSettingsMenuViewControllersInstaller* self) {
    MainSettingsInstaller_InstallBindings(self);
    reactive::beatsaber::g_menuContainer = self->get_Container();
}

MAKE_HOOK_MATCH(BSAppInit_InstallBindings,
                &BeatSaber::Init::BSAppInit::InstallBindings, void,
                BeatSaber::Init::BSAppInit* self) {
    self->::UnityEngine::MonoBehaviour::StartCoroutine(
        custom_types::Helpers::CoroutineHelper::New(
            reactive::beatsaber::BundleLoader::LoadBundle(self->get_gameObject())));
    BSAppInit_InstallBindings(self);
}

MAKE_HOOK_MATCH(Image_get_pixelsPerUnit,
                &UnityEngine::UI::Image::get_pixelsPerUnit, float,
                UnityEngine::UI::Image* self) {
    float result = Image_get_pixelsPerUnit(self);
    auto cast = il2cpp_utils::try_cast<Reactive::BeatSaber::FixedImageView>(self);
    if (cast != std::nullopt) {
        result *= self->pixelsPerUnitMultiplier;
    }
    return result;
}

MAKE_HOOK_MATCH(VRGraphicRaycaster_RaycastCanvas,
                &VRUIControls::VRGraphicRaycaster::RaycastCanvas, void,
                UnityEngine::Canvas* canvas, UnityEngine::Ray ray,
                float hitDistance, float curvedUIRadius,
                System::Collections::Generic::List_1<VRUIControls::VRGraphicRaycaster::VRGraphicRaycastResult>* results) {
    VRGraphicRaycaster_RaycastCanvas(canvas, ray, hitDistance, curvedUIRadius, results);

    int i = 0;
    while (i < results->get_Count()) {
        auto raycast = results->get_Item(i);
        auto graphic = raycast.graphic;
        // cordl has no MaskableGraphic, so this covers every graphic.
        if (graphic) {
            auto masks = graphic->GetComponentsInParent<Reactive::BeatSaber::RaycastMask2D*>(false);
            bool valid = true;
            for (size_t k = 0; k < masks.size(); ++k) {
                auto* mask = masks[k];
                if (!mask) continue;
                if (!mask->ValidatePoint({raycast.insideRootCanvasPosition.x,
                                          raycast.insideRootCanvasPosition.y})) {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                results->RemoveAt(i);
                continue;
            }
        }
        ++i;
    }
}

// Runs after every LateUpdate and before UGUI rebuilds, like PC's Canvas.preWillRenderCanvases subscription.
MAKE_HOOK_MATCH(Canvas_SendPreWillRenderCanvases,
                &UnityEngine::Canvas::SendPreWillRenderCanvases, void) {
    Canvas_SendPreWillRenderCanvases();
    reactive::ReactiveScheduler::Run();
}

MOD_EXPORT void setup(CModInfo* info) noexcept {
    *info = modInfo.to_c();
    Paper::Logger::RegisterFileContextId(ReactiveLogger.tag);
}

MOD_EXPORT "C" void late_load() {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();

    // custom-types stores the C++ token "Reactive::BeatSaber", but the bundle's MonoScript references "Reactive.BeatSaber".
    for (auto* klass : custom_types::Register::classes) {
        if (!klass->namespaze || std::strcmp(klass->namespaze, "Reactive::BeatSaber") != 0) continue;
        for (const char* name : {"BeatSaberResources", "SpriteCollection", "MaterialCollection"}) {
            if (std::strcmp(klass->name, name) == 0) klass->namespaze = "Reactive.BeatSaber";
        }
    }

    ReactiveLogger.info("Reactive v" VERSION " loaded.");

    INSTALL_HOOK(ReactiveLogger, MainSettingsInstaller_InstallBindings);
    INSTALL_HOOK(ReactiveLogger, BSAppInit_InstallBindings);
    INSTALL_HOOK(ReactiveLogger, Image_get_pixelsPerUnit);
    INSTALL_HOOK(ReactiveLogger, VRGraphicRaycaster_RaycastCanvas);
    INSTALL_HOOK(ReactiveLogger, Canvas_SendPreWillRenderCanvases);
}
