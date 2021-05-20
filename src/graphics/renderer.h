#pragma once

#include <glm/vec3.hpp>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"
#include "camera.h"

class RendererGL2 {
public:
    static void set_window_hints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    }

    static void begin_frame(const Store& store) {
        {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            auto camera_position =
                store.get<glm::vec3>(StoreParams::kCameraPosition);
            auto camera_target =
                store.get<glm::vec3>(StoreParams::kCameraTarget);
            auto camera_up = store.get<glm::vec3>(StoreParams::kCameraUp);
            gluLookAt(camera_position.x, camera_position.y, camera_position.z,
                      camera_target.x, camera_target.y, camera_target.z,
                      camera_up.x, camera_up.y, camera_up.z);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(
                glm::degrees(store.get<f32>(StoreParams::kCameraFov)),
                store.get<f32>(StoreParams::kCameraAspectRatio),
                store.get<f32>(StoreParams::kCameraNear),
                store.get<f32>(StoreParams::kCameraFar));
        }

        glViewport(0, 0, store.get<u32>(StoreParams::kFrameBufferWidth),
                   store.get<u32>(StoreParams::kFrameBufferHeight));

        glClearColor(0.5f, 0.5f, 0.5f, 1.f);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if (Settings::graphics_multisampling) {
            glEnable(GL_MULTISAMPLE);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    static void render_gizmo(const glm::vec3& position) {
        glBegin(GL_LINES);

        auto edge = 1.f;

        glLineWidth(8.f);

        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(position.x + edge, position.y, position.z);

        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(position.x, position.y + edge, position.z);

        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(position.x, position.y, position.z + edge);

        glLineWidth(1.f);

        glEnd();
    }

    static void render_grid(const glm::vec3& position, f32 size,
                            const glm::vec3& color) {
        glBegin(GL_LINES);

        auto sizeOver2 = size / 2.f;

        glLineWidth(8.f);
        glColor3f(color.r, color.g, color.b);

        const int count = 10;
        for (auto i = 0; i <= count; ++i) {
            glVertex3f(position.x - sizeOver2, position.y,
                       position.z - sizeOver2 + i * (size / count));
            glVertex3f(position.x + sizeOver2, position.y,
                       position.z - sizeOver2 + i * (size / count));
        }

        for (auto i = 0; i <= count; ++i) {
            glVertex3f(position.x - sizeOver2 + i * (size / count), position.y,
                       position.z - sizeOver2);
            glVertex3f(position.x - sizeOver2 + i * (size / count), position.y,
                       position.z + sizeOver2);
        }

        glLineWidth(1.f);

        glEnd();
    }
};

class RendererGL4 {
public:
    static void set_window_hints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
};