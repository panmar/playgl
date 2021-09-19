#pragma once

#include "common.h"
#include "graphics/opengl/texture.h"
#include "graphics/opengl/framebuffer.h"
#include "graphics/opengl/shader.h"
#include "graphics/opengl/geometry_renderer.h"
#include "content.h"

// clang-format off
//
// EXAMPLES:
//
//     postprocess("#main")
//        .with("inverse.fs")
//        .param("intensity", 15.f)
//        .resulting("#inverse");
//
// clang-format on

class Postprocess {
public:
    Postprocess(Content& content, GeometryRenderer& geometry_renderer,
                FramebufferContainer& framebuffers)
        : content(content),
          geometry_renderer(geometry_renderer),
          framebuffers(framebuffers) {}

    Postprocess& operator()(const string& id) { return framebuffer(id); }

    Postprocess& with(const string& fragment_shader_id) {
        shader = &content.shader("postprocess.vs", fragment_shader_id);
        return *this;
    }

    template <class ParamType>
    Postprocess& param(const char* name, const ParamType& value) {
        if (!shader) {
            throw std::runtime_error(
                "Postprocess: `with` argument should be passed before "
                "`param`");
        }
        shader->param(name, value);
        return *this;
    }

    void resulting(const string& output_framebuffer_id) {
        if (input_framebuffer_id.empty()) {
            throw std::runtime_error(
                "Postprocess: missing `framebuffer` argument");
        }

        if (!shader) {
            throw std::runtime_error("Postprocess: missing `with` argument");
        }

        framebuffers(output_framebuffer_id).color().bind();

        geometry_renderer.render(
            geometry::ScreenQuad{},
            shader->param(
                "tex",
                framebuffers(input_framebuffer_id).color_texture.value()));

        command_cleanup();
    }

private:
    Content& content;
    GeometryRenderer& geometry_renderer;
    FramebufferContainer& framebuffers;

    string input_framebuffer_id;
    Shader* shader = nullptr;

    Postprocess& framebuffer(const string& id) {
        input_framebuffer_id = id;
        return *this;
    }

    void command_cleanup() {
        input_framebuffer_id = "";
        shader = nullptr;
    }
};
