#pragma once

#include <thread>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "camera.h"
#include "gui.h"
#include "input.h"
#include "settings.h"
#include "store.h"
#include "timer.h"

#include "scene/example_scene.h"

void on_key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action,
                     i32 mods);

void on_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset);
void on_cursor_position_callback(GLFWwindow* window, f64 x, f64 y);
void on_mouse_button_callback(GLFWwindow* window, i32 button, i32 action,
                              i32 mods);

class PlayGlApp {
public:
    void run() {
        using namespace std::literals::chrono_literals;

        if (!startup()) {
            return;
        }

        while (!glfwWindowShouldClose(window)) {
            input.update();
            glfwPollEvents();

            timer.tick();
            store.set(StoreParams::kTimeElapsedSeconds,
                      timer.get_elapsed_seconds());

            camera_controller.update(camera, input);
            store.set(StoreParams::kCameraPosition, camera.get_position());
            store.set(StoreParams::kCameraTarget, camera.get_target());
            store.set(StoreParams::kCameraUp, camera.get_up());
            store.set(StoreParams::kCameraView, camera.get_view());
            store.set(StoreParams::kCameraFov, camera.get_fov());
            store.set(StoreParams::kCameraAspectRatio,
                      camera.get_aspect_ratio());
            store.set(StoreParams::kCameraNear, camera.get_near());
            store.set(StoreParams::kCameraFar, camera.get_far());
            store.set(StoreParams::kCameraProjection, camera.get_projection());

            Scene::update(store);
            Gui::update(store);

            Scene::render(store);
            Gui::render(store);

            glfwSwapBuffers(window);
            if (input.is_key_pressed(Settings::key_quit)) {
                glfwSetWindowShouldClose(window, true);
            }

            std::this_thread::sleep_for(16ms);
        }

        shutdown();
    }

    void on_key_changed(i32 key, i32 scancode, i32 action, i32 mods) {
        input.on_key_changed(key, scancode, action, mods);
    }

    void on_scroll(f64 x_offset, f64 y_offset) {
        input.scroll_x_offsets.push_back(x_offset);
        input.scroll_y_offsets.push_back(y_offset);
    }

    void on_cursor_position(f64 x, f64 y) {
        input.cursor_pos_x = x;
        input.cursor_pos_y = y;
    }

    void on_mouse_button_changed(i32 key, i32 action, i32 mods) {
        input.on_mouse_button_changed(key, action, mods);
    }

private:
    GLFWwindow* window = nullptr;
    Timer timer;
    Input input;
    Store store;
    PerspectiveCamera camera;
    OrbitCameraController camera_controller;

    bool startup() {
        if (!glfwInit()) {
            return false;
        }

        Scene::pre_graphics_startup();

        window = glfwCreateWindow(Settings::graphics_resolution_width,
                                  Settings::graphics_resolution_height,
                                  Settings::window_title, nullptr, nullptr);

        store.set(StoreParams::kFrameBufferWidth,
                  Settings::graphics_resolution_width);
        store.set(StoreParams::kFrameBufferHeight,
                  Settings::graphics_resolution_height);

        if (!window) {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            return false;
        }

        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, on_key_callback);
        glfwSetScrollCallback(window, ::on_scroll_callback);
        glfwSetCursorPosCallback(window, ::on_cursor_position_callback);
        glfwSetMouseButtonCallback(window, ::on_mouse_button_callback);

        return true;
    }

    void shutdown() {}
};

inline void on_key_callback(GLFWwindow* window, i32 key, i32 scancode,
                            i32 action, i32 mods) {
    auto app = reinterpret_cast<PlayGlApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->on_key_changed(key, scancode, action, mods);
    }
}

inline void on_scroll_callback(GLFWwindow* window, f64 x_offset, f64 y_offset) {
    auto app = reinterpret_cast<PlayGlApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->on_scroll(x_offset, y_offset);
    }
}

inline void on_cursor_position_callback(GLFWwindow* window, f64 x, f64 y) {
    auto app = reinterpret_cast<PlayGlApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->on_cursor_position(x, y);
    }
}

inline void on_mouse_button_callback(GLFWwindow* window, i32 button, i32 action,
                                     i32 mods) {
    auto app = reinterpret_cast<PlayGlApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->on_mouse_button_changed(button, action, mods);
    }
}
