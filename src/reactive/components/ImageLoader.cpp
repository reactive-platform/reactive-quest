#include "reactive/components/ImageLoader.hpp"

#include <unordered_map>
#include <vector>

#include <System/Action_1.hpp>
#include <UnityEngine/AsyncOperation.hpp>
#include <UnityEngine/HideFlags.hpp>
#include <UnityEngine/Networking/DownloadHandlerTexture.hpp>
#include <UnityEngine/Networking/UnityWebRequest.hpp>
#include <UnityEngine/Networking/UnityWebRequestAsyncOperation.hpp>
#include <UnityEngine/Networking/UnityWebRequestTexture.hpp>
#include <UnityEngine/Rect.hpp>
#include <UnityEngine/SpriteMeshType.hpp>
#include <UnityEngine/Texture2D.hpp>
#include <UnityEngine/Vector2.hpp>
#include <UnityEngine/Vector4.hpp>

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "custom-types/shared/delegate.hpp"
#include "main.hpp"

namespace reactive::components {
namespace {
    using Request = UnityEngine::Networking::UnityWebRequest;

    struct CachedImage {
        SafePtrUnity<UnityEngine::Sprite>                      sprite;
        std::vector<std::function<void(UnityEngine::Sprite*)>> waiters;
    };

    std::unordered_map<std::string, CachedImage> g_images;

    UnityEngine::Sprite* CreateSprite(Request* request) {
        auto* texture = UnityEngine::Networking::DownloadHandlerTexture::GetContent(request).unsafePtr();
        if (!texture) return nullptr;

        UnityEngine::Sprite* sprite = UnityEngine::Sprite::Create(
            texture, UnityEngine::Rect{0.f, 0.f, (float)texture->get_width(), (float)texture->get_height()},
            UnityEngine::Vector2{0.5f, 0.5f}, 100.f, 1u, UnityEngine::SpriteMeshType::FullRect,
            UnityEngine::Vector4{0.f, 0.f, 0.f, 0.f}, false);

        // Nothing managed refers to them while no image shows them, and a scene change would unload them.
        texture->set_hideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
        sprite->set_hideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
        return sprite;
    }

    void HandleCompleted(std::string const& url, Request* request) {
        UnityEngine::Sprite* sprite = nullptr;
        if (request->get_result() == Request::Result::Success) {
            sprite = CreateSprite(request);
        } else {
            ReactiveLogger.warn("Failed to load {}: {}", url, static_cast<std::string>(request->get_error()));
        }
        request->Dispose();

        auto& image = g_images[url];
        if (sprite) image.sprite = sprite;

        // Moved out first, because a callback can request an image again.
        auto waiters = std::move(image.waiters);
        image.waiters.clear();
        for (auto& waiter : waiters) waiter(sprite);
    }
}

void LoadImage(std::string const& url, std::function<void(UnityEngine::Sprite*)> callback) {
    auto& image = g_images[url];
    if (image.sprite) {
        callback(image.sprite.ptr());
        return;
    }

    image.waiters.push_back(std::move(callback));
    if (image.waiters.size() > 1) return;

    SafePtr<Request> request(UnityEngine::Networking::UnityWebRequestTexture::GetTexture(StringW(url)));
    request->SendWebRequest()->add_completed(custom_types::MakeDelegate<System::Action_1<UnityEngine::AsyncOperation*>*>(
        std::function<void(UnityEngine::AsyncOperation*)>([url, request](UnityEngine::AsyncOperation*) mutable {
            HandleCompleted(url, request.ptr());
        })));
}
}
