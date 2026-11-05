#include "reactive/beatsaber/BeatSaberResources.hpp"

#include <algorithm>

#include <System/Collections/IEnumerator.hpp>
#include <UnityEngine/AssetBundleCreateRequest.hpp>

#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "main.hpp"

namespace reactive::beatsaber {
namespace {
    constexpr auto kMainAssetName = "BeatSaberSDK_Resources";
}

UnityEngine::AssetBundle*           BundleLoader::assetBundle = nullptr;
Reactive::BeatSaber::BundleHost*    BundleLoader::host        = nullptr;

custom_types::Helpers::Coroutine BundleLoader::LoadBundle(UnityEngine::GameObject* container) {
    if (assetBundle == NULL) {
        ArrayW<uint8_t> bytes(reactive_bundle::getLength());
        std::copy(reactive_bundle::getData(),
                  reactive_bundle::getData() + reactive_bundle::getLength(),
                  bytes.begin());

        using AssetBundle_LoadFromMemoryAsync =
            function_ptr_t<UnityEngine::AssetBundleCreateRequest*, ArrayW<uint8_t>, int>;
        static auto assetBundle_LoadFromMemoryAsync =
            reinterpret_cast<AssetBundle_LoadFromMemoryAsync>(
                il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemoryAsync_Internal"));

        auto req = assetBundle_LoadFromMemoryAsync(bytes, 0);
        req->set_allowSceneActivation(true);
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(req);

        assetBundle = req->get_assetBundle();
        // Null when a bundle with the same name is already loaded, because Unity derives its internal id from the name.
        if (!assetBundle) {
            ReactiveLogger.error("Failed to load the Reactive asset bundle");
            co_return;
        }
    }

    auto type   = csTypeOf(Reactive::BeatSaber::BeatSaberResources*);
    auto loaded = assetBundle->LoadAsset(kMainAssetName, type);
    auto* resources = reinterpret_cast<Reactive::BeatSaber::BeatSaberResources*>(loaded.unsafePtr());
    if (!resources) {
        ReactiveLogger.error("BeatSaberResources: '{}' did not resolve as the SDK container",
                             kMainAssetName);
        co_return;
    }

    host = container->AddComponent<Reactive::BeatSaber::BundleHost*>();
    host->resources = resources;
    co_return;
}
}
