#pragma once

#include <glm/vec3.hpp>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "../common.h"
#include "../camera.h"

class RendererGL2 {
public:
    static void setup(const Store& store) {
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(glm::degrees(store.get<f32>("CAMERA_FOV")),
                           store.get<f32>("CAMERA_ASPECT_RATIO"),
                           store.get<f32>("CAMERA_NEAR"),
                           store.get<f32>("CAMERA_FAR"));
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            auto camera_position = store.get<glm::vec3>("CAMERA_POSITION");
            auto camera_target = store.get<glm::vec3>("CAMERA_TARGET");
            auto camera_up = store.get<glm::vec3>("CAMERA_UP");
            gluLookAt(camera_position.x, camera_position.y, camera_position.z,
                      camera_target.x, camera_target.y, camera_target.z,
                      camera_up.x, camera_up.y, camera_up.z);
        }

        glViewport(0, 0, store.get<i32>("GRAPHICS_FRAMEBUFFER_WIDTH"),
                   store.get<i32>("GRAPHICS_FRAMEBUFFER_HEIGHT"));

        glClearColor(0.5f, 0.5f, 0.5f, 1.f);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // if (Settings::graphics_multisampling) {
        //     glEnable(GL_MULTISAMPLE);
        // }

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
};