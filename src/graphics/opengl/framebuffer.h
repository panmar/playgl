#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"
#include "graphics/opengl/texture.h"
#include "graphics/opengl/geometry_renderer.h"
#include "content.h"

class Framebuffer {
public:
    Framebuffer(Framebuffer&& other)
        : content(other.content),
          geometry_renderer(other.geometry_renderer),
          framebuffer(other.framebuffer),
          color_texture(std::move(other.color_texture)),
          depth_texture(std::move(other.depth_texture)) {
        other.framebuffer = 0;
        other.color_texture = std::nullopt;
        other.depth_texture = std::nullopt;
    }

    Framebuffer(Content& content, GeometryRenderer& geometry_renderer)
        : content(content), geometry_renderer(geometry_renderer) {
        glGenFramebuffers(1, &framebuffer);
    }

    ~Framebuffer() {
        if (framebuffer) {
            glDeleteFramebuffers(1, &framebuffer);
        }
    }

    Framebuffer& clear(const Color& color) {
        bind();

        glClearColor(color.r, color.g, color.b, color.a);
        auto clear_flags = color_texture.has_value() ? GL_COLOR_BUFFER_BIT : 0;
        clear_flags |= depth_texture.has_value() ? GL_DEPTH_BUFFER_BIT : 0;
        glClear(clear_flags);
        return *this;
    }

    Framebuffer& present() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // TODO: make those values from camera dimentions
        glViewport(0, 0, 1920, 1200);

        geometry_renderer.render(
            geometry::ScreenQuad{},
            content.shader("postprocess.vs", "postprocess.fs")
                .param("tex", color_texture.value()));
        return *this;
    }

    Framebuffer& color() {
        if (!framebuffer) {
            std::runtime_error("No valid framebuffer found");
        }

        if (color_texture) {
            return *this;
        }

        color_texture.emplace(Texture::from_desc(
            TextureDesc{1920, 1200, TextureDesc::Format::RGB}));
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, color_texture.value().resource(),
                               0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            std::runtime_error("Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return *this;
    }

    Framebuffer& depth() {
        if (!framebuffer) {
            std::runtime_error("No valid framebuffer found");
        }

        if (depth_texture) {
            return *this;
        }

        depth_texture.emplace(Texture::from_desc(
            TextureDesc{1920, 1200, TextureDesc::Format::Depth32}));
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depth_texture.value().resource(),
                               0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            std::runtime_error("Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return *this;
    }

    void bind() const {
        if (framebuffer) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            // TODO: make those values from color_texture desc
            glViewport(0, 0, 1920, 1200);
        }
    }

    void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    Content& content;
    GeometryRenderer& geometry_renderer;
    u32 framebuffer = 0;
    optional<Texture> color_texture;
    optional<Texture> depth_texture;
};

class FramebufferContainer {
public:
    FramebufferContainer(Content& content, GeometryRenderer& geometry_renderer)
        : content(content), geometry_renderer(geometry_renderer) {}

    Framebuffer& operator()(const string& framebuffer_id) {
        return get(framebuffer_id);
    }

private:
    Framebuffer& get(const string& framebuffer_id) {
        if (id_to_framebuffers.count(framebuffer_id)) {
            return id_to_framebuffers.find(framebuffer_id)->second;
        }

        return id_to_framebuffers
            .insert({framebuffer_id, Framebuffer{content, geometry_renderer}})
            .first->second;
    }

    Content& content;
    GeometryRenderer& geometry_renderer;
    unordered_map<string, Framebuffer> id_to_framebuffers;
};