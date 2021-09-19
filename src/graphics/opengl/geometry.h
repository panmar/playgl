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
    vector<u32> indices;
    Topology topology = Topology::Triangles;
};

namespace geometry {

struct Grid : public Geometry {
    Grid() {
        topology = Topology::Lines;

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

        positions = {
            {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};
        normals = {
            {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};
        texcoords = {{1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}};
    };
};

// positions:
//----------
// [-1,1]   |    [1,1]
//          |
// ------------------
//          |
// [-1,-1]  |    [1,-1]
//
// texcoords:
//----------
// [0,1]    |    [1,1]
//          |
// ------------------
//          |
// [0,0]    |    [1,0]
//
struct Quad : public Geometry {
    Quad() {
        topology = Topology::Triangles;

        // clang-format off
        positions = {
            {-1.f, 0.f, 1.f}, {-1.f, 0.f, -1.f}, {1.f, 0.f, -1.f},
            {-1.f, 0.f, 1.f}, {1.f, 0.f, -1.f}, {1.f, 0.f, 1.f}
        };
        normals = {
            {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
            {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}
        };
        texcoords = {
            {0.f, 1.f}, {0.f, 0.f}, {1.f, 0.f},
            {0.f, 1.f}, {1.f, 0.f}, {1.f, 1.f}
        };
        // clang-format on
    }
};	

struct ScreenQuad : public Geometry {
    ScreenQuad() {
        topology = Topology::Triangles;

        // clang-format off
        positions = {
            {-1.f, 1.f, 0.f}, {-1.f, -1.f, 0.f}, {1.f, -1.f, 0.f},
            {-1.f, 1.f, 0.f}, {1.f, -1.f, 0.f}, {1.f, 1.f, 0.f}
        };
        normals = {
            {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f},
            {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f}
        };
        texcoords = {
            {0.f, 1.f}, {0.f, 0.f}, {1.f, 0.f},
            {0.f, 1.f}, {1.f, 0.f}, {1.f, 1.f}
        };
        // clang-format on
    }
};

struct Cube : public Geometry {
    Cube() {
        topology = Topology::Triangles;

        // clang-format off
        positions = {
            {-1.f, 0.f, 1.f}, {-1.f, 0.f, -1.f}, {1.f, 0.f, -1.f},
            {-1.f, 0.f, 1.f}, {1.f, 0.f, -1.f}, {1.f, 0.f, 1.f}
        };
        normals = {
            {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
            {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}
        };
        texcoords = {
            {0.f, 1.f}, {0.f, 0.f}, {1.f, 0.f},
            {0.f, 1.f}, {1.f, 0.f}, {1.f, 1.f}
        };
        // clang-format on
    }
};

struct WireCube : public Geometry {
    WireCube() {
        topology = Topology::Lines;

        vec3 corners[8] = {vec3(-0.5f, -0.5f, 0.5f),  vec3(0.5f, -0.5f, 0.5f),
                           vec3(0.5f, 0.5f, 0.5f),    vec3(-0.5f, 0.5f, 0.5f),
                           vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f),
                           vec3(0.5f, 0.5f, -0.5f),   vec3(-0.5f, 0.5f, -0.5f)};

        positions = {corners[0], corners[1], corners[1], corners[2], corners[2],
                     corners[3], corners[3], corners[0], corners[4], corners[5],
                     corners[5], corners[6], corners[6], corners[7], corners[7],
                     corners[4], corners[0], corners[4], corners[1], corners[5],
                     corners[2], corners[6], corners[3], corners[7]};
    }
};

struct WireSphere : public Geometry {
    WireSphere(u32 slices, u32 stacks) {
        topology = Topology::Lines;

        if (stacks % 2 != 0) {
            ++stacks;
        }

        f32 delta_theta = glm::radians(360.f / slices);
        f32 delta_phi = glm::radians(180.f / stacks);

        // NOTE(panmar): Populate latitude lines (stacks)

        f32 phi = -glm::pi<f32>() / 2.f;
        for (u32 i = 0; i < stacks; ++i) {
            vector<vec3> v;
            phi += delta_phi;
            f32 theta = 0;
            for (u32 j = 0; j <= slices; ++j) {
                theta += delta_theta;
                f32 x = cos(phi) * cos(theta);
                f32 y = sin(phi);
                f32 z = cos(phi) * sin(theta);
                v.push_back(vec3(x, y, z));
            }

            for (u32 k = 0; k < v.size() - 1; ++k) {
                positions.push_back(v[k]);
                positions.push_back(v[k + 1]);
            }
        }

        // NOTE(panmar): Populate longitude lines (slices)

        f32 theta = 0.f;
        for (u32 i = 0; i <= slices; ++i) {
            vector<vec3> v;
            theta += delta_theta;
            f32 phi = 0.f;
            for (u32 j = 0; j <= 2 * stacks; ++j) {
                phi += delta_phi;
                f32 x = cos(phi) * cos(theta);
                f32 y = sin(phi);
                f32 z = cos(phi) * sin(theta);
                v.push_back(vec3(x, y, z));
            }
            for (u32 k = 0; k < v.size() - 1; ++k) {
                positions.push_back(v[k]);
                positions.push_back(v[k + 1]);
            }
        }
    }
};

struct WirePyramid : public Geometry {
    WirePyramid() {
        topology = Topology::Lines;

        f32 edge = 1.f;
        f32 edgeOver2 = edge / 2.0f;
        f32 height = edge * glm::sqrt(2.f) / 2.0f;
        f32 heightOver2 = height / 2.0f;

        positions = {{-edgeOver2, -heightOver2, edgeOver2},
                     {edgeOver2, -heightOver2, edgeOver2},
                     {edgeOver2, -heightOver2, -edgeOver2},
                     {-edgeOver2, -heightOver2, -edgeOver2},
                     {0, heightOver2, 0}};

        indices = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 4, 2, 4, 3, 4};
    }
};

}  // namespace geometry
