#pragma once

#include "common.h"
#include "graphics/texture.h"
#include "graphics/framebuffer.h"
#include "graphics/shader.h"
#include "graphics/geometry_renderer.h"
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

    template <class... StringType>
    Postprocess& operator()(StringType&... ids) {
        return framebuffer({convert(ids)...});
    }

    Postprocess& with(const string& fragment_shader_id) {
        shader = &content.shader("postprocess.vs", fragment_shader_id);
        return *this;
    }

    template <class ParamType>
    Postprocess& param(const char* name, const ParamType& value) {
        if (!shader) {
            throw PlayGlException(
                "Postprocess: `with` argument should be passed before "
                "`param`");
        }
        shader->param(name, value);
        return *this;
    }

    void resulting(Framebuffer& framebuffer) {
        DEBUG_SCOPE("postprocess");

        if (input_framebuffers.empty()) {
            throw PlayGlException(
                "Postprocess: missing `framebuffer` argument");
        }

        if (!shader) {
            throw PlayGlException("Postprocess: missing `with` argument");
        }

        framebuffer.color().bind();

        u32 i = 0;
        for (auto& framebuffer : input_framebuffers) {
            auto param_name = "tex" + std::to_string(i);
            shader->param(param_name.c_str(),
                          framebuffer->color_texture.value());
            ++i;
        }

        geometry_renderer(geometry::ScreenQuad{})
            .shader(*shader)
            .param("transform", mat4(1.f))
            .state(GpuState().nodepth())
            .render();

        command_cleanup();
    }

    void resulting(const string& output_framebuffer_id) {
        resulting(framebuffers(output_framebuffer_id));
    }

private:
    Postprocess& framebuffer(const vector<Framebuffer*>& framebuffers) {
        input_framebuffers = framebuffers;
        return *this;
    }

    void command_cleanup() {
        input_framebuffers.clear();
        shader = nullptr;
    }

    Framebuffer* convert(const string& id) { return &framebuffers(id); }

    Framebuffer* convert(Framebuffer& framebuffer) { return &framebuffer; }

    Content& content;
    GeometryRenderer& geometry_renderer;
    FramebufferContainer& framebuffers;

    vector<Framebuffer*> input_framebuffers;
    Shader* shader = nullptr;
};
