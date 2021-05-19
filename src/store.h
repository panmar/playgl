#pragma once

#include <variant>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "common.h"

namespace StoreParams {

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

    template <class T>
    void set(const string& key, const T& value) {
        key_values.insert_or_assign(key, value);
    }

    template <class T>
    T get(const string& name) const {
        auto it = key_values.find(name);
        if (it == key_values.end()) {
        }
        auto& value = it->second;
        return std::get<T>(value);
    }

private:
    using Value = std::variant<i32, u32, f32, glm::vec3, glm::mat4>;
    std::map<std::string, Value> key_values;
};
