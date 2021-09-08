#pragma once

#include <variant>

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"

class StoreParam {
public:
    operator i32&() { return std::get<i32>(param); }
    operator f32&() { return std::get<f32>(param); }
    operator vec3&() { return std::get<vec3>(param); }
    operator vec4&() { return std::get<vec4>(param); }
    operator mat4&() { return std::get<mat4>(param); }
    operator string&() { return std::get<string>(param); }
    operator const char*() { return std::get<string>(param).c_str(); }

    template <typename T>
    StoreParam& operator=(const T& param) {
        this->param = param;
        return *this;
    }

    // NOTE(panmar): To avoid confusion and reduce number of errors
    // we do store unsigned integers as signed integers;
    template <>
    StoreParam& operator=(const u32& param) {
        this->param = static_cast<i32>(param);
        return *this;
    }

private:
    using ParamType =
        std::variant<i32, f32, glm::vec3, glm::vec4, glm::mat4, string>;
    ParamType param;
};

class Store {
public:
    enum class ParamAnnotation { Gui, Shader, ReadOnly };

    StoreParam& operator[](const string& name) {
        auto it = key_values.find(name);
        if (it == key_values.end()) {
            it = key_values.insert({name, StoreParam()}).first;
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, StoreParam> key_values;
};

inline Store& pgl_store() {
    static Store store;
    return store;
}

#define STORE pgl_store()

// NOTE(panmar): Build-in store params
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

inline void initialize() {
    STORE[kWindowTitle] = "PlayGL";
    STORE[kFrameBufferWidth] = 1920;
    STORE[kFrameBufferHeight] = 1200;

    STORE[kKeyQuit] = GLFW_KEY_ESCAPE;
    STORE[kKeyCameraRotate] = GLFW_MOUSE_BUTTON_RIGHT;
}

}  // namespace StoreParams