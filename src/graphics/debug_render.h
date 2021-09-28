#pragma once

#include "common.h"
#include "content.h"
#include "graphics/geometry.h"
#include "graphics/geometry_renderer.h"
#include "graphics/camera.h"
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
    DEBUG_DESC_PROPERTY(GridDesc, Color, color) = Color(1.f, 1.f, 1.f, 1.f);

    // NOTE(panmar): Should it persist across frames?
    DEBUG_DESC_PROPERTY_DEFAULT(GridDesc, bool, permanent, true) = false;
};

struct GizmoDesc {
    DEBUG_DESC_PROPERTY(GizmoDesc, vec3, scale) = vec3(1.f);
    DEBUG_DESC_PROPERTY(GizmoDesc, mat4, rotation) = mat4(1.f);
    DEBUG_DESC_PROPERTY(GizmoDesc, vec3, position) = vec3(0.f);
    DEBUG_DESC_PROPERTY(GizmoDesc, mat4, transform) = mat4(1.f);
    DEBUG_DESC_PROPERTY_DEFAULT(GizmoDesc, bool, permanent, true) = false;
};

struct ModelDesc {
    ModelDesc(const string& model_id) : model_id(model_id) {}
    DEBUG_DESC_PROPERTY_DEFAULT(ModelDesc, bool, permanent, true) = false;

    string model_id;
};

class DebugRenderer {
public:
    DebugRenderer(Content& content, GeometryRenderer& renderer)
        : content(content), geometry_renderer(renderer) {}

    GridDesc& grid() const { return grids.emplace_back(); }
    GizmoDesc& gizmo() const { return gizmos.emplace_back(); }
    ModelDesc& model(const string& model_id) const {
        return models.emplace_back(model_id);
    }

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
        models.erase(std::remove_if(models.begin(), models.end(),
                                    [](const ModelDesc& item) {
                                        return !item._permanent;
                                    }),
                     models.end());
    }

    void render(const Camera& camera) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, config::window_width, config::window_height);

        glClear(GL_DEPTH_BUFFER_BIT);

        render_grids(camera);
        render_gizmos(camera);
        render_models(camera);
    }

private:
    Content& content;
    GeometryRenderer& geometry_renderer;
    mutable vector<GridDesc> grids;
    mutable vector<GizmoDesc> gizmos;
    mutable vector<ModelDesc> models;

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
                GpuState());
        }
    }

    void render_gizmos(const Camera& camera) {
        for (auto& gizmo : gizmos) {
            geometry_renderer.render(
                geometry::Gizmo{},
                debug_shader
                    .param("world",
                           gizmo._transform * glm::translate(gizmo._position) *
                               gizmo._rotation * glm::scale(gizmo._scale))
                    .param("view", camera.geometry.get_view())
                    .param("projection", camera.geometry.get_projection())
                    .param("color", vec4(1.f, 0.f, 0.f, 1.f))
                    .param("resolution",
                           vec2(camera.canvas.width, camera.canvas.height))
                    .param("dash_size", 10.f)
                    .param("gap_size", 10.f),
                GpuState());
        }
    }

    void render_models(const Camera& camera) {
        for (auto& model_desc : models) {
            auto gpu_state = GpuState().wireframe().nodepth();

            auto model = content.model(model_desc.model_id).resource();
            for (auto& part : model.parts) {
                geometry_renderer.render(
                    part.geometry,
                    content.shader("solid_color.vs", "solid_color.fs")
                        .param("world", part.transform)
                        .param("view", camera.geometry.get_view())
                        .param("projection", camera.geometry.get_projection())
                        .param("color", Colors::Green),
                    GpuState().wireframe());
            }
        }
    }
};

}  // namespace debug
