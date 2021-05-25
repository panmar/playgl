#pragma once

#include <variant>

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"

namespace StoreParams {

const string kWindowTitle = "WINDOW_TITLE";
const string kKeyQuit = "KEY_QUIT";
const string kKeyCameraRotate = "KEY_CAMERA_ROTATE";

const string kFrameBufferWidth = "GRAPHICS_FRAMEBUFFER_WIDTH";
const string kFrameBufferHeight = "GRAPHICS_FRAMEBUFFER_HEIGHT";

const string kTimeElapsedSeconds = "TIME_ELAPSED_SECONDS";

const string kCameraView = "CAMERA_VIEW";
const string kCameraProjection = "CAMERA_PROJECTION";
const string kCameraPosition = "CAMERA_POSITION";
const string kCameraTarget = "CAMERA_TARGET";
const string kCameraUp = "CAMERA_UP";
const string kCameraFov = "CAMERA_FOV";
const string kCameraAspectRatio = "CAMERA_ASPECT_RATIO";
const string kCameraNear = "CAMERA_NEAR";
const string kCameraFar = "CAMERA_FAR";

}  // namespace StoreParams

class Store {
public:
    enum class ParamFlag { Gui, Shader, ReadOnly };

    Store() {
        set(StoreParams::kWindowTitle, "PlayGL");
        set(StoreParams::kKeyQuit, GLFW_KEY_ESCAPE);
        set(StoreParams::kKeyCameraRotate, GLFW_MOUSE_BUTTON_RIGHT);

        auto framebuffer_width = 800;
        auto framebuffer_height = 600;

        set(StoreParams::kFrameBufferWidth, framebuffer_width);
        set(StoreParams::kFrameBufferHeight, framebuffer_height);

        auto camera_position = glm::vec3{10.f, 10.f, 10.f};
        auto camera_target = glm::vec3{0.f, 0.f, 0.f};
        auto camera_up = glm::normalize(
            glm::cross(glm::normalize(camera_target - camera_position),
                       glm::normalize(glm::vec3{1.f, 2.f, 3.f})));

        set(StoreParams::kCameraPosition, camera_position);
        set(StoreParams::kCameraTarget, camera_target);
        set(StoreParams::kCameraUp, camera_up);
        set(StoreParams::kCameraFov, 1.1623f);
        set(StoreParams::kCameraAspectRatio,
            (static_cast<f32>(framebuffer_width) / framebuffer_height));
        set(StoreParams::kCameraNear, 0.1f);
        set(StoreParams::kCameraFar, 100.f);
    }

    template <class T>
    void set(const string& key, const T& value) {
        key_values.insert_or_assign(key, value);
    }

    // NOTE(panmar): To avoid confusion and reduce number of errors
    // we do store unsigned integers as signed integers;
    // When the user stores value as signed int and retrieves the same value
    // as unsigned int (and vice versa) there will be no errors provided all
    // parameters fall into signed integer range
    template <>
    void set(const string& key, const u32& value) {
        key_values.insert_or_assign(key, static_cast<i32>(value));
    }

    template <class T>
    T get(const string& name) const {
        auto it = key_values.find(name);
        if (it == key_values.end()) {
        }
        auto& value = it->second;
        return std::get<T>(value);
    }

    template <>
    u32 get(const string& name) const {
        auto it = key_values.find(name);
        if (it == key_values.end()) {
        }
        auto& value = it->second;
        return static_cast<u32>(std::get<i32>(value));
    }

private:
    using Value = std::variant<i32, f32, glm::vec3, glm::mat4, string>;
    std::unordered_map<std::string, Value> key_values;
};

inline Store& pgl_store() {
    static Store store;
    return store;
}
