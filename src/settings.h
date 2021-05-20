#pragma once

#include "common.h"

namespace Settings {

const char* window_title = "PlayGL";

const u32 graphics_resolution_width = 1920;
const u32 graphics_resolution_height = 1080;
const bool graphics_fullscreen = false;
const bool graphics_borderless = false;
const bool graphics_multisampling = true;
const i32 graphics_multisampling_samples = 4;

const i32 key_rotate_left = GLFW_KEY_LEFT;
const i32 key_rotate_right = GLFW_KEY_RIGHT;
const i32 key_rotate_up = GLFW_KEY_UP;
const i32 key_rotate_down = GLFW_KEY_DOWN;
const i32 key_zoom_in = GLFW_KEY_COMMA;
const i32 key_zoom_out = GLFW_KEY_PERIOD;
const i32 key_quit = GLFW_KEY_ESCAPE;
const i32 key_camera_center = GLFW_KEY_C;
const i32 key_camera_rotate = GLFW_MOUSE_BUTTON_RIGHT;

};  // namespace Settings