#pragma once

#include "_config.hpp"

#include "scotland2/shared/loader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "paper2_scotland2/shared/logger.hpp"

inline modloader::ModInfo modInfo = {MOD_ID, VERSION, 0};

constexpr auto ReactiveLogger = Paper::ConstLoggerContext("Reactive");
