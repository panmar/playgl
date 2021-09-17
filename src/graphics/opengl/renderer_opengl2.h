#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"
#include "camera.h"
#include "model.h"

class RendererGL2 {
public:
    static void set_window_hints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    }

    static void begin_frame(const Camera& camera,
                            bool enable_multisampling = false) {
        {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            auto camera_position = camera.geometry.get_position();
            auto camera_target = camera.geometry.get_target();
            auto camera_up = camera.geometry.get_up();
            gluLookAt(camera_position.x, camera_position.y, camera_position.z,
                      camera_target.x, camera_target.y, camera_target.z,
                      camera_up.x, camera_up.y, camera_up.z);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(glm::degrees(camera.geometry.get_fov()),
                           camera.geometry.get_aspect_ratio(),
                           camera.geometry.get_near(),
                           camera.geometry.get_far());
        }

        glViewport(0, 0, camera.canvas.width, camera.canvas.height);

        glClearColor(0.5f, 0.5f, 0.5f, 1.f);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if (enable_multisampling) {
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
                            const Color& color) {
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

    static void render_model(const Model& model) {
        glm::vec4 color{0.1f, 0.1f, 0.1f, 1.f};

        ModelData data = model.data;

        for (auto& node : data.current_model_graph) {
            auto absolute_transform =
                data.get_absolute_node_transform(node.name);
            for (int i = 0; i < node.model_parts.size(); ++i) {
                auto& model_part = data.model_parts[node.model_parts[i]];
                if (model_part.bones.empty()) {
                    render_model_part(model_part, absolute_transform,
                                      model.data.bounding_sphere.radius, color);
                } else {
                    render_skinned_model_part(
                        model_part, data.get_skinning_palette(model_part.bones),
                        model.data.bounding_sphere.radius, color);
                }
            }
        }
    }

    static void render_model_part(const ModelPart& model_part,
                                  const glm::mat4& absolute_transform,
                                  f32 scale, const Color& color) {
        glPolygonMode(GL_FRONT, GL_LINE);
        glColor4f(color.r, color.g, color.b, color.a);
        std::vector<glm::vec4> pos;
        auto world = glm::mat4(1.f);
        world = glm::scale(world, glm::vec3(scale));
        for (auto& p : model_part.positions) {
            auto p4 =
                world * absolute_transform * glm::vec4(p.x, p.y, p.z, 1.f);
            pos.push_back(p4);
        }

        glBegin(GL_TRIANGLES);
        for (auto& i : model_part.indices) {
            glVertex4f(pos[i].x, pos[i].y, pos[i].z, pos[i].w);
        }
        glEnd();

        glPolygonMode(GL_FRONT, GL_FILL);
    }

    static void render_skinned_model_part(
        const ModelPart& model_part,
        const std::vector<glm::mat4>& skinning_palette, f32 scale,
        const Color& color) {
        glPolygonMode(GL_FRONT, GL_LINE);

        glColor4f(color.r, color.g, color.b, color.a);
        glm::vec3 light_dir = glm::normalize(glm::vec3(1.f, 1.f, 1.f));
        // glPolygonMode(GL_FRONT, GL_NONE);
        glBegin(GL_TRIANGLES);
        for (auto& index : model_part.indices) {
            auto pos = model_part.positions[index];
            auto blend_indices = model_part.joints[index];
            auto blend_weights = model_part.weights[index];

            glm::mat4 xform =
                skinning_palette[blend_indices.v[0]] * blend_weights[0];
            xform += skinning_palette[blend_indices.v[1]] * blend_weights[1];
            xform += skinning_palette[blend_indices.v[2]] * blend_weights[2];
            xform += skinning_palette[blend_indices.v[3]] * blend_weights[3];

            auto world = glm::mat4(1.f);
            world = glm::scale(world, glm::vec3(1.f / scale));

            glm::vec4 final_pos = world * xform * glm::vec4(pos, 1.f);
            glVertex4f(final_pos.x, final_pos.y, final_pos.z, final_pos.w);
        }
        glEnd();

        glPolygonMode(GL_FRONT, GL_FILL);
    }
};