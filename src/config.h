#pragma once

#include "common.h"

namespace config {

using namespace std::literals::chrono_literals;

const char* window_title = "playgl";
auto window_width = 1920;
auto window_height = 1200;

auto key_quit = GLFW_KEY_ESCAPE;
auto key_camera_rotate = GLFW_MOUSE_BUTTON_RIGHT;

constexpr auto multisampling = true;
constexpr auto inverse_depth = true;
constexpr auto frame_time = 32ms;

auto gamma = 2.2f;

}  // namespace config