#pragma once

#include <HMUI/ImageView.hpp>

#include "custom-types/shared/macros.hpp"

// No explicit ctor: HMUI::ImageView's own ctor sets up the curved canvas helper, and calling it by hand leaves that unset.
DECLARE_CLASS_CODEGEN(Reactive::BeatSaber, FixedImageView, HMUI::ImageView) {};
