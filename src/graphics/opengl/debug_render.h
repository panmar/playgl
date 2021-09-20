#pragma once

#include "common.h"
#include "content.h"
#include "graphics/opengl/geometry.h"
#include "graphics/opengl/geometry_renderer.h"
#include "store.h"

namespace debug {

#define DEBUG_DESC_INTERNAL_FIELD(property_name) _##property_name

#define DEBUG_DESC_PROPERTY(desc_type, property_type, property_name) \
    desc_type& property_name(const property_type& value) {           \
        DEBUG_DESC_INTERNAL_FIELD(property_name) = value;            \
        return *this;                                                \
    }                                                                \
    property_type DEBUG_DESC_INTERNAL_FIELD(property_name)

#define DEBUG_DESC_PROPERTY_DEFAULT(desc_type, property_type, property_name, \
                                    default_value)                           \
    desc_type& property_name(const property_type& value = default_value) {   \
        DEBUG_DESC_INTERNAL_FIELD(property_name) = value;                    \
        return *this;                                                        \
    }                                                                        \
    property_type DEBUG_DESC_INTERNAL_FIELD(property_name)

struct GridDesc {
    DEBUG_DESC_PROPERTY(GridDesc, vec3, position) = vec3(0.f, 0.f, 0.f);
    DEBUG_DESC_PROPERTY(GridDesc, vec3, normal) = vec3(0.f, 1.f, 0.f);
    DEBUG_DESC_PROPERTY(GridDesc, f32, edge) = 1.f;
    DEBUG_DESC_PROPERTY(GridDesc, Color, color) = vec4(1.f, 1.f, 1.f, 1.f);

    // NOTE(panmar): Should it persist across frames?
    DEBUG_DESC_PROPERTY_DEFAULT(GridDesc, bool, permanent, true) = false;
    // NOTE(panmar): Should it use depth information when rendering?
    DEBUG_DESC_PROPERTY_DEFAULT(GridDesc, bool, depth, true) = false;
};

struct GizmoDesc {
    DEBUG_DESC_PROPERTY(GizmoDesc, vec3, position) = vec3(0.f, 0.f, 0.f);
    DEBUG_DESC_PROPERTY_DEFAULT(GizmoDesc, bool, permanent, true) = false;
    DEBUG_DESC_PROPERTY_DEFAULT(GizmoDesc, bool, depth, true) = false;
};

class DebugRenderer {
public:
    DebugRenderer(GeometryRenderer& renderer)
        : geometry_renderer(renderer) {}

    GridDesc& grid() const { return grids.emplace_back(); }
    GizmoDesc& gizmo() const { return gizmos.emplace_back(); }

    void clear_ephemerals() {
        grids.erase(std::remove_if(
                        grids.begin(), grids.end(),
                        [](const GridDesc& item) { return !item._permanent; }),
                    grids.end());
        gizmos.erase(std::remove_if(gizmos.begin(), gizmos.end(),
                                    [](const GizmoDesc& item) {
                                        return !item._permanent;
                                    }),
                     gizmos.end());
    }

    void render(const Camera& camera) {
        render_grids(camera);
        render_gizmos(camera);
    }

private:
    GeometryRenderer& geometry_renderer;
    mutable vector<GridDesc> grids;
    mutable vector<GizmoDesc> gizmos;

    Shader debug_shader = Shader::from_file("data\\shaders\\debug_dash.vs",
                                            "data\\shaders\\debug_dash.fs");

    void render_grids(const Camera& camera) {
        for (auto& grid : grids) {
            geometry_renderer.render(
                geometry::Grid{},
                debug_shader
                    .param("world", glm::translate(grid._position) *
                                        glm::scale(vec3(grid._edge / 2.f)))

                    .param("view", camera.geometry.get_view())
                    .param("projection", camera.geometry.get_projection())
                    .param("color", grid._color)
                    .param("resolution",
                           vec2(camera.canvas.width, camera.canvas.height))
                    .param("dash_size", 10.f)
                    .param("gap_size", 10.f),
                GpuState().nodepth());
        }
    }

    void render_gizmos(const Camera& camera) {
        for (auto& gizmo : gizmos) {
            geometry_renderer.render(
                geometry::Gizmo{},
                debug_shader.param("world", glm::translate(gizmo._position))
                    .param("view", camera.geometry.get_view())
                    .param("projection", camera.geometry.get_projection())
                    .param("color", vec4(1.f, 0.f, 0.f, 1.f))
                    .param("resolution",
                           vec2(camera.canvas.width, camera.canvas.height))
                    .param("dash_size", 10.f)
                    .param("gap_size", 10.f),
                GpuState().nodepth());
        }
    }
};

}  // namespace debug
