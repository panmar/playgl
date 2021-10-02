#pragma once

#include <thread>

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"
#include "config.h"
#include "graphics/graphics.h"
#include "gui.h"
#include "input.h"
#include "store.h"
#include "timer.h"

struct System {
    Timer timer;
    Input input;
    Store store;
    Content content = Content(std::filesystem::path{"data/"});

    Camera camera;
    OrbitCameraController camera_controller;

    GeometryRenderer geometry{content, store};
    FramebufferContainer framebuffers{content};

    debug::DebugRenderer debug{content, geometry, framebuffers("#__debug__")};

    Postprocess postprocess{content, geometry, framebuffers};
};

// NOTE(panmar): Those functions should be defined by extending program
// ---------------------------------
// Called ONCE before window/graphics setup
void pgl_init(Store& store);

// Called every frame
void pgl_update(System& system);
void pgl_render(System& system);

// ----------------------------------

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
            auto frame_timer = Timer{};
            system.input.update();
            glfwPollEvents();

            system.timer.tick();
            system.debug.clear_ephemerals();
            system.camera_controller.update(system.camera, system.input);

            pgl_update(system);

            // NOTE(panmar): Not sure if I need to clear the cache here;
            // ImGui seems to clean after itself
            // GpuStateCache::clear();
            {
                DEBUG_SCOPE("debug-clear");
                system.debug.clear();
            }

            {
                DEBUG_SCOPE("camera-clear");
                system.camera.canvas.framebuffer =
                    &system.framebuffers("#main").color().depth();
                system.camera.canvas.clear();
            }

            {
                DEBUG_SCOPE("pgl_render");
                pgl_render(system);
            }

            {
                DEBUG_SCOPE("debug");
                system.debug.render(system.camera);
            }

            {
                DEBUG_SCOPE("gamma-correction");
                system.postprocess(*system.camera.canvas.framebuffer)
                    .with("gamma_correction.fs")
                    .param("gamma", config::gamma)
                    .resulting("#gamma_corrected");
                system.framebuffers("#gamma_corrected").bind();
            }

            {
                DEBUG_SCOPE("imgui");
                Gui::render(system.store);
            }

            {
                DEBUG_SCOPE("present");
                system.framebuffers("#gamma_corrected").present();
            }

            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);

            if (system.input.is_key_pressed(config::key_quit)) {
                glfwSetWindowShouldClose(window, true);
            }

            std::this_thread::sleep_until(frame_timer.get_tick_time() +
                                          config::frame_time);
        }

        shutdown();
    }

    void on_key_changed(i32 key, i32 scancode, i32 action, i32 mods) {
        system.input.on_key_changed(key, scancode, action, mods);
    }

    void on_scroll(f64 x_offset, f64 y_offset) {
        system.input.scroll_x_offsets.push_back(x_offset);
        system.input.scroll_y_offsets.push_back(y_offset);
    }

    void on_cursor_position(f64 x, f64 y) {
        system.input.cursor_pos_x = x;
        system.input.cursor_pos_y = y;
    }

    void on_mouse_button_changed(i32 key, i32 action, i32 mods) {
        system.input.on_mouse_button_changed(key, action, mods);
    }

    void on_framebuffer_resize(u32 width, u32 height) {
        if (!width || !height) {
            return;
        }

        system.camera.canvas.width = width;
        system.camera.canvas.height = height;
        system.camera.geometry.set_aspect_ratio(width /
                                                static_cast<f32>(height));

        config::window_width = width;
        config::window_height = height;
    }

private:
    GLFWwindow* window = nullptr;
    System system;

    bool startup() {
        if (!glfwInit()) {
            return false;
        }

        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        }
        pgl_init(system.store);

        window = glfwCreateWindow(config::window_width, config::window_height,
                                  config::window_title, nullptr, nullptr);

        if (!window) {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            return false;
        }

        debug::setup_logging();

        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, on_key_callback);
        glfwSetScrollCallback(window, ::on_scroll_callback);
        glfwSetCursorPosCallback(window, ::on_cursor_position_callback);
        glfwSetMouseButtonCallback(window, ::on_mouse_button_callback);
        glfwSetFramebufferSizeCallback(window, ::on_framebuffer_resize);

        if (!Gui::startup(window)) {
            return false;
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