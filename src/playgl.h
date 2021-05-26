#pragma once

#include <thread>

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "camera.h"
#include "gui.h"
#include "input.h"
#include "store.h"
#include "timer.h"

#include "content.h"

// NOTE(panmar): Those functions should be defined by extending program
void pgl_init();
void pgl_update();
void pgl_render();

void on_key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action,
                     i32 mods);
void on_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset);
void on_cursor_position_callback(GLFWwindow* window, f64 x, f64 y);
void on_mouse_button_callback(GLFWwindow* window, i32 button, i32 action,
                              i32 mods);
void on_framebuffer_resize(GLFWwindow* window, i32 width, i32 height);

class PlayGlApp {
public:
    void run() {
        using namespace std::literals::chrono_literals;

        pgl_init();

        if (!startup()) {
            return;
        }

        // NOTE(panmar): Even if we set glfwSetFramebufferSizeCallback callback
        // during ::StartUp(), for some reason it is not triggered when
        // framebuffer is initialized, so, at the beginning, we need to trigger
        // it manually
        {
            i32 framebuffer_width, framebuffer_height;
            glfwGetFramebufferSize(window, &framebuffer_width,
                                   &framebuffer_height);
            on_framebuffer_resize(framebuffer_width, framebuffer_height);
        }

        while (!glfwWindowShouldClose(window)) {
            input.update();
            glfwPollEvents();

            timer.tick();
            pgl_store().set(StoreParams::kTimeElapsedSeconds,
                            timer.get_elapsed_seconds());

            camera_controller.update(camera, input);
            {
                pgl_store().set(StoreParams::kCameraPosition,
                                camera.get_position());
                pgl_store().set(StoreParams::kCameraTarget,
                                camera.get_target());
                pgl_store().set(StoreParams::kCameraUp, camera.get_up());
                pgl_store().set(StoreParams::kCameraView, camera.get_view());
                pgl_store().set(StoreParams::kCameraFov, camera.get_fov());
                pgl_store().set(StoreParams::kCameraAspectRatio,
                                camera.get_aspect_ratio());
                pgl_store().set(StoreParams::kCameraNear, camera.get_near());
                pgl_store().set(StoreParams::kCameraFar, camera.get_far());
                pgl_store().set(StoreParams::kCameraProjection,
                                camera.get_projection());
            }

            pgl_update();
            Gui::update();

            pgl_render();
            Gui::render();

            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);

            if (input.is_key_pressed(
                    pgl_store().get<i32>(StoreParams::kKeyQuit))) {
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

    void on_framebuffer_resize(u32 width, u32 height) {
        pgl_store().set(StoreParams::kFrameBufferWidth, width);
        pgl_store().set(StoreParams::kFrameBufferHeight, height);
        camera.set_aspect_ratio(static_cast<f32>(width) / height);
    }

private:
    GLFWwindow* window = nullptr;
    Timer timer;
    Input input;
    PerspectiveCamera camera;
    OrbitCameraController camera_controller;

    bool startup() {
        if (!glfwInit()) {
            return false;
        }

        pgl_init();

        window = glfwCreateWindow(
            pgl_store().get<i32>(StoreParams::kFrameBufferWidth),
            pgl_store().get<i32>(StoreParams::kFrameBufferHeight),
            pgl_store().get<string>(StoreParams::kWindowTitle).c_str(), nullptr,
            nullptr);

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
        glfwSetFramebufferSizeCallback(window, ::on_framebuffer_resize);

        if (!Gui::startup(window)) {
            return false;
        }

        {
            camera.set_position(
                pgl_store().get<glm::vec3>(StoreParams::kCameraPosition));
            camera.set_target(
                pgl_store().get<glm::vec3>(StoreParams::kCameraTarget));
            camera.set_up(pgl_store().get<glm::vec3>(StoreParams::kCameraUp));
            camera.set_aspect_ratio(
                pgl_store().get<f32>(StoreParams::kCameraAspectRatio));
            camera.set_fov(pgl_store().get<f32>(StoreParams::kCameraFov));
            camera.set_near(pgl_store().get<f32>(StoreParams::kCameraNear));
            camera.set_far(pgl_store().get<f32>(StoreParams::kCameraFar));
        }

        return true;
    }

    void shutdown() {
        Gui::shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
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

inline void on_framebuffer_resize(GLFWwindow* window, i32 width, i32 height) {
    auto app = reinterpret_cast<PlayGlApp*>(glfwGetWindowUserPointer(window));
    if (app) {
        if (width != 0 && height != 0) {
            app->on_framebuffer_resize(static_cast<u32>(width),
                                       static_cast<u32>(height));
        }
    }
}

#ifdef PGL_DEFINE_MAIN
int main() {
    PlayGlApp app;
    app.run();
    return 0;
}
#endif