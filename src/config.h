#pragma once

#include "common.h"

namespace config {

const char* window_title = "playgl";
u32 window_width = 800;
u32 window_height = 600;

u32 key_quit = GLFW_KEY_ESCAPE;
u32 key_camera_rotate = GLFW_MOUSE_BUTTON_RIGHT;

constexpr bool inverse_depth = true;

}  // namespace config