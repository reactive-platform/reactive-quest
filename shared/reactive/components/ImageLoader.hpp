#pragma once

#include <functional>
#include <string>

#include <UnityEngine/Sprite.hpp>

namespace reactive::components {
// Each url is downloaded once and its sprite kept for good. The callback runs on the main thread, right away for a
// cached image, and gets null if the download failed.
void LoadImage(std::string const& url, std::function<void(UnityEngine::Sprite*)> callback);
}
