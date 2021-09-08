#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "camera.h"

class RendererGL4 {
public:
    static void set_window_hints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    static void begin_frame(const Camera& camera, bool enable_multisampling) {
        glViewport(0, 0, camera.canvas.width, camera.canvas.height);
        glClearColor(camera.canvas.color.r, camera.canvas.color.g,
                     camera.canvas.color.b, camera.canvas.color.a);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if (enable_multisampling) {
            glEnable(GL_MULTISAMPLE);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};