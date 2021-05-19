#pragma once

#include <glm/vec3.hpp>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "../common.h"
#include "../camera.h"

class BasicRenderer {
public:
    ~BasicRenderer() {}

    void render(const PerspectiveCamera& camera) {
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(glm::degrees(camera.get_fov()),
                           camera.get_aspect_ratio(), camera.get_near(),
                           camera.get_far());
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(camera.get_position().x, camera.get_position().y,
                      camera.get_position().z, camera.get_target().x,
                      camera.get_target().y, camera.get_target().z,
                      camera.get_up().x, camera.get_up().y, camera.get_up().z);
        }


        glViewport(0, 0, framebuffer_width, framebuffer_height);

        glClearColor(0.5f, 0.5f, 0.5f, 1.f);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // if (Settings::graphics_multisampling) {
        //     glEnable(GL_MULTISAMPLE);
        // }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_gizmo(glm::vec3(0.f, 0.f, 0.f));
    }

    void render_gizmo(const glm::vec3& position) {
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

    void set_framebuffer_width(i32 value) { framebuffer_width = value; }
    void set_framebuffer_height(i32 value) { framebuffer_height = value; }

private:
    i32 framebuffer_width = 0;
    i32 framebuffer_height = 0;
};