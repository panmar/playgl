#pragma once

#include <thread>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "camera.h"
#include "gui.h"
#include "input.h"
#include "scene.h"
#include "store.h"
#include "timer.h"

void on_key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action,
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
            store.set("TIME_ELAPSED_SECONDS", timer.get_elapsed_seconds());

            camera_controller.update(&camera, input);
            store.set("CAMERA_POSITION", camera.get_position());
            store.set("CAMERA_TARGET", camera.get_target());
            store.set("CAMERA_UP", camera.get_up());
            store.set("CAMERA_VIEW", camera.get_view());
            store.set("CAMERA_FOV", camera.get_fov());
            store.set("CAMERA_ASPECT_RATIO", camera.get_aspect_ratio());
            store.set("CAMERA_NEAR", camera.get_near());
            store.set("CAMERA_FAR", camera.get_far());
            store.set("CAMERA_PROJECTION", camera.get_projection());

            scene.update(store);
            gui.update(store);

            scene.render(store);
            gui.render(store);

            glfwSwapBuffers(window);
            std::this_thread::sleep_for(16ms);
        }

        shutdown();
    }

    void on_key_changed(i32 key, i32 scancode, i32 action, i32 mods) {
        input.on_key_changed(key, scancode, action, mods);
    }

private:
    GLFWwindow* window = nullptr;
    Timer timer;
    Input input;
    Store store;
    Gui gui;
    PerspectiveCamera camera;
    OrbitCameraController camera_controller;
    Scene scene;

    bool startup() {
        if (!glfwInit()) {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 600, "PlayGL", nullptr, nullptr);

        store.set("GRAPHICS_FRAMEBUFFER_WIDTH", 800);
        store.set("GRAPHICS_FRAMEBUFFER_HEIGHT", 600);

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

        return true;
    }

    void shutdown() {}
};

inline void on_key_callback(GLFWwindow* window, i32 key, i32 scancode,
                            i32 action, i32 mods) {
    auto app = reinterpret_cast<PlayGlApp*>(glfwGetWindowUserPointer);
    if (app) {
        app->on_key_changed(key, scancode, action, mods);
    }
}
