#pragma once

#include "common.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

struct Geometry {
    enum class Topology { Triangles = GL_TRIANGLES, Lines = GL_LINES };

    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> texcoords;

    Topology topology = Topology::Triangles;
    u32 vertex_start_index = 0;
    u32 vertex_count = 0;
};

namespace geometry {

struct Grid : public Geometry {
    Grid() {
        topology = Topology::Lines;
        vertex_count = 20;

        // clang-format off
        positions = {
                {-1.f, 0.f, -1.f}, {-1.f, 0.f, 1.f},
                {-0.5f, 0.f, -1.f}, {-0.5f, 0.f, 1.f},
                {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f},
                {0.5f, 0.f, -1.f}, {0.5f, 0.f, 1.f},
                {1.f, 0.f, -1.f}, {1.f, 0.f, 1.0f},
                
                {-1.f, 0.f, -1.f}, {1.f, 0.f, -1.f},
                {-1.f, 0.f, -0.5f}, {1.f, 0.f, -0.5f},
                {-1.f, 0.f, 0.f}, {1.f, 0.f, 0.f},
                {-1.f, 0.f, 0.5f}, {1.f, 0.f, 0.5f},
                {-1.f, 0.f, 1.f}, {1.f, 0.f, 1.0f}
            };
        // clang-format on
    }
};

struct Gizmo : public Geometry {
    Gizmo() {
        topology = Topology::Lines;
        vertex_count = 6;

        // clang-format off
        positions = {
                {1.f, 0.f, 0.f}, {0.f, 0.f, 0.f},
                {0.f, 1.f, 0.f}, {0.f, 0.f, 0.f},
                {0.f, 0.f, 1.f}, {0.f, 0.f, 0.f},
            };
        // clang-format on
    }
};

struct Triangle : public Geometry {
    Triangle() {
        topology = Topology::Triangles;
        vertex_count = 3;

        positions = {
            {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};
        normals = {
            {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};
        texcoords = {{-0.5f, -0.5f}, {0.5f, -0.5f}, {0.0f, 0.5f}};
    };
};

}  // namespace geometry
