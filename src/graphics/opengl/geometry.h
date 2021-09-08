#pragma once

#include "common.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

struct Geometry {
    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> texcoords;
};

class GpuData {
public:
    ~GpuData() { clean(); }

    void create(const Geometry& geometry) {
        clean();

        vector<vec3> merged_geometry_data;
        // TODO(panmar): Fill the data

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     2 * sizeof(vec3) * geometry.positions.size(),
                     merged_geometry_data.data(), GL_STATIC_DRAW);

        // NOTE(panmar): This part is about the expected shader layout
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(f32), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(f32),
                              (void*)(3 * sizeof(f32)));
    }

    void bind() {
        assert(vao != 0);
        glBindVertexArray(vao);
    }

    void unbind() { glBindVertexArray(0); }

private:
    void clean() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
        }

        if (vbo) {
            glDeleteBuffers(1, &vbo);
        }
    }

    u32 vbo = 0;
    u32 vao = 0;
};