#pragma once

#include "common.h"
#include "content.h"
#include "graphics/geometry.h"
#include "graphics/geometry_renderer.h"
#include "graphics/camera.h"
#include "graphics/framebuffer.h"
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
    DebugRenderer(Content& content, GeometryRenderer& renderer,
                  Framebuffer& debug_layer)
        : content(content),
          geometry_renderer(renderer),
          debug_layer(debug_layer) {}

    void clear() {
        debug_layer.color().depth().clear(Color(1.f, 1.f, 1.f, 0.f));
        debug_textures_drawn = 0;
    }

    GridDesc& grid() const { return grids.emplace_back(); }
    GizmoDesc& gizmo() const { return gizmos.emplace_back(); }
    ModelDesc& model(const string& model_id) const {
        return models.emplace_back(model_id);
    }

    void texture(const Texture& texture) {
        DEBUG_SCOPE("debug - texture render");

        if (debug_textures_drawn >= MAX_DEBUG_TEXTURES) {
            throw PlayGlException("Too many debug textures.");
        }

        auto active_framebuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &active_framebuffer);
        i32 viewport_desc[4] = {0};
        glGetIntegerv(GL_VIEWPORT, viewport_desc);

        {
            debug_layer.bind();

            auto height = debug_layer.color_texture.value().desc.height /
                          MAX_DEBUG_TEXTURES;
            auto width = height * texture.desc.aspect_ratio();

            geometry_renderer(geometry::ScreenQuad())
                .shader("postprocess.vs", "postprocess.fs")
                .param("transform",
                       create_transform(
                           0, height * debug_textures_drawn, width, height,
                           debug_layer.color_texture.value().desc.width,
                           debug_layer.color_texture.value().desc.height))
                .param("tex0", texture)
                .render();
        }

        ++debug_textures_drawn;

        glBindFramebuffer(GL_FRAMEBUFFER, active_framebuffer);
        GpuStateCache::glViewport(viewport_desc[0], viewport_desc[1],
                                  viewport_desc[2], viewport_desc[3]);
    }

    static mat4 create_transform(f32 x, f32 y, f32 width, f32 height,
                                 f32 screen_width, f32 screen_height) {
        auto scale_width = 1.f * width / screen_width;
        auto scale_height = 1.f * height / screen_height;
        auto x_ndc = -1.f + 2.f * x / screen_width + (width / screen_width);
        auto y_ndc = 1.f - 2.f * y / screen_height - (height / screen_height);
        return glm::translate(vec3(x_ndc, y_ndc, 1.f)) *
               glm::scale(vec3(scale_width, scale_height, 0.f));
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
        debug_layer.bind();

        render_grids(camera);
        render_gizmos(camera);
        render_models(camera);

        render_to_camera(camera);
    }

private:
    Content& content;
    GeometryRenderer& geometry_renderer;
    Framebuffer& debug_layer;

    mutable vector<GridDesc> grids;
    mutable vector<GizmoDesc> gizmos;
    mutable vector<ModelDesc> models;

    u32 debug_textures_drawn = 0;
    static constexpr u32 MAX_DEBUG_TEXTURES = 7;

    void render_grids(const Camera& camera) {
        for (auto& grid : grids) {
            geometry_renderer(geometry::Grid{})
                .shader("debug_dash.vs", "debug_dash.fs")
                .param("world", glm::translate(grid._position) *
                                    glm::scale(vec3(grid._edge / 2.f)))
                .param("view", camera.geometry.get_view())
                .param("projection", camera.geometry.get_projection())
                .param("color", grid._color)
                .param("resolution",
                       vec2(camera.canvas.width, camera.canvas.height))
                .param("dash_size", 10.f)
                .param("gap_size", 10.f)
                .render();
        }
    }

    void render_gizmos(const Camera& camera) {
        for (auto& gizmo : gizmos) {
            geometry_renderer(geometry::Gizmo{})
                .shader("debug_dash.vs", "debug_dash.fs")
                .param("world", gizmo._transform *
                                    glm::translate(gizmo._position) *
                                    gizmo._rotation * glm::scale(gizmo._scale))
                .param("view", camera.geometry.get_view())
                .param("projection", camera.geometry.get_projection())
                .param("color", vec4(1.f, 0.f, 0.f, 1.f))
                .param("resolution",
                       vec2(camera.canvas.width, camera.canvas.height))
                .param("dash_size", 10.f)
                .param("gap_size", 10.f)
                .render();
        }
    }

    void render_models(const Camera& camera) {
        for (auto& model_desc : models) {
            auto gpu_state = GpuState().wireframe().nodepth();

            auto model = content.model(model_desc.model_id).resource();
            for (auto& part : model.parts) {
                geometry_renderer(part.geometry)
                    .shader("solid_color.vs", "solid_color.fs")
                    .param("world", part.transform)
                    .param("view", camera.geometry.get_view())
                    .param("projection", camera.geometry.get_projection())
                    .param("color", Colors::Green)
                    .state(GpuState().wireframe())
                    .render();
            }
        }
    }

    void render_to_camera(const Camera& camera) {
        camera.canvas.framebuffer->bind();
        geometry_renderer(geometry::ScreenQuad())
            .shader("postprocess.vs", "postprocess.fs")
            .param("transform", mat4(1.f))
            .param("tex0", debug_layer.color_texture.value())
            .state(GpuState().nodepth().blend(
                GpuState::BlendMode::SrcAlpha,
                GpuState::BlendMode::OneMinusSrcAlpha))
            .render();
    }
};

}  // namespace debug
