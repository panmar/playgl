#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define PAR_SHAPES_T u32
#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>

#include "common.h"

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

struct PlatonicSolidParShapes : public Geometry {
    using MeshGenerator = par_shapes_mesh* (*)();

    PlatonicSolidParShapes(MeshGenerator generator) {
        topology = Topology::Triangles;

        par_shapes_mesh* mesh = generator();
        par_shapes_unweld(mesh, true);
        par_shapes_compute_normals(mesh);

        for (auto i = 0; i < mesh->npoints; ++i) {
            positions.push_back({mesh->points[3 * i], mesh->points[3 * i + 1],
                                 mesh->points[3 * i + 2]});
        }

        if (mesh->normals) {
            for (auto i = 0; i < mesh->npoints; ++i) {
                normals.push_back({mesh->normals[3 * i],
                                   mesh->normals[3 * i + 1],
                                   mesh->normals[3 * i + 2]});
            }
        }

        auto mesh_index_count = mesh->ntriangles * 3;
        for (auto i = 0; i < mesh_index_count; ++i) {
            u32 index = mesh->triangles[i];
            indices.push_back(index);
        }

        par_shapes_free_mesh(mesh);
    }
};

struct Dodecahedron : public PlatonicSolidParShapes {
    Dodecahedron() : PlatonicSolidParShapes(par_shapes_create_dodecahedron) {}
};

struct Tetrahedron : public PlatonicSolidParShapes {
    Tetrahedron() : PlatonicSolidParShapes(par_shapes_create_tetrahedron) {}
};

struct Octahedron : public PlatonicSolidParShapes {
    Octahedron() : PlatonicSolidParShapes(par_shapes_create_octahedron) {}
};

struct Isohedron : public PlatonicSolidParShapes {
    Isohedron() : PlatonicSolidParShapes(par_shapes_create_icosahedron) {}
};

struct ParametricSurfaceParShapes : public Geometry {
    template <class MeshGeneratorType, class... Args>
    ParametricSurfaceParShapes(MeshGeneratorType generator, Args... args) {
        topology = Topology::Triangles;

        par_shapes_mesh* mesh = generator(args...);

        for (auto i = 0; i < mesh->npoints; ++i) {
            positions.push_back({mesh->points[3 * i], mesh->points[3 * i + 1],
                                 mesh->points[3 * i + 2]});
        }

        if (mesh->normals) {
            for (auto i = 0; i < mesh->npoints; ++i) {
                normals.push_back({mesh->normals[3 * i],
                                   mesh->normals[3 * i + 1],
                                   mesh->normals[3 * i + 2]});
            }
        }

        if (mesh->tcoords) {
            for (auto i = 0; i < mesh->npoints; ++i) {
                texcoords.push_back(
                    {mesh->tcoords[2 * i], mesh->tcoords[2 * i + 1]});
            }
        }

        auto mesh_index_count = mesh->ntriangles * 3;
        for (auto i = 0; i < mesh_index_count; ++i) {
            u32 index = mesh->triangles[i];
            indices.push_back(index);
        }

        par_shapes_free_mesh(mesh);
    }
};

template <u32 slices = 30, u32 stacks = 3>
struct OpenCylinder : public ParametricSurfaceParShapes {
    OpenCylinder()
        : ParametricSurfaceParShapes(par_shapes_create_cylinder, slices,
                                     stacks) {}
};

template <u32 slices = 1, u32 stacks = 30>
struct CylinderDisk : public ParametricSurfaceParShapes {
    CylinderDisk(const vec3& center = {0, 1, 3}, const vec3& normal = {0, 1, 0})
        : ParametricSurfaceParShapes(par_shapes_create_disk, slices, stacks,
                                     &center.x, &normal.x) {}
};

template <u32 slices = 10, u32 stacks = 10>
struct Hemisphere : public ParametricSurfaceParShapes {
    Hemisphere()
        : ParametricSurfaceParShapes(par_shapes_create_hemisphere, slices,
                                     stacks) {}
};

template <u32 slices = 3, u32 stacks = 3>
struct Plane : public ParametricSurfaceParShapes {
    Plane()
        : ParametricSurfaceParShapes(par_shapes_create_plane, slices, stacks) {}
};

template <u32 slices = 30, u32 stacks = 40>
struct Torus : public ParametricSurfaceParShapes {
    Torus(f32 radius = 1.f)
        : ParametricSurfaceParShapes(par_shapes_create_torus, slices, stacks,
                                     radius) {}
};

template <u32 slices = 15, u32 stacks = 15>
struct Sphere : public ParametricSurfaceParShapes {
    Sphere()
        : ParametricSurfaceParShapes(par_shapes_create_parametric_sphere,
                                     slices, stacks) {}
};

template <u32 subdivisions = 1>
struct SubdividedSphere : public ParametricSurfaceParShapes {
    SubdividedSphere()
        : ParametricSurfaceParShapes(par_shapes_create_subdivided_sphere,
                                     subdivisions) {}
};

template <u32 slices = 30, u32 stacks = 40>
struct TrefoilKnot : public ParametricSurfaceParShapes {
    TrefoilKnot(f32 radius = 1.f)
        : ParametricSurfaceParShapes(par_shapes_create_trefoil_knot, slices,
                                     stacks, radius) {}
};

inline Geometry extract_normals(const Geometry& geometry) {
    if (geometry.normals.empty()) {
        throw PlayGlException("Geometry does not have any normals");
    }

    Geometry result;
    result.topology = Geometry::Topology::Lines;

    for (auto i = 0; i < geometry.positions.size(); ++i) {
        result.positions.push_back(geometry.positions[i]);
        result.positions.push_back(geometry.positions[i] + geometry.normals[i]);
    }

    result.indices.resize(result.normals.size());
    std::iota(result.indices.begin(), result.indices.end(), 0);

    return result;
}

}  // namespace geometry
