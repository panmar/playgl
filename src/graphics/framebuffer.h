#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"
#include "config.h"
#include "graphics/texture.h"
#include "content.h"

class Framebuffer : public LazyResource<u32> {
public:
    Framebuffer(Framebuffer&& other)
        : content(other.content),
          color_texture(std::move(other.color_texture)),
          depth_texture(std::move(other.depth_texture)),
          LazyResource(std::move(other)) {
        other.color_texture = std::nullopt;
        other.depth_texture = std::nullopt;
    }

    Framebuffer(Content& content)
        : content(content), LazyResource(resource_deleter) {}

    Framebuffer& clear(const Color& color = Colors::Black) {
        bind();

        glClearColor(color.r, color.g, color.b, color.a);
        if (config::inverse_depth) {
            glClearDepthf(0.f);
        } else {
            glClearDepthf(1.f);
        }

        auto clear_flags = color_texture.has_value() ? GL_COLOR_BUFFER_BIT : 0;
        clear_flags |= depth_texture.has_value() ? GL_DEPTH_BUFFER_BIT : 0;
        glClear(clear_flags);
        return *this;
    }

    Framebuffer& present() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, resource());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, color_texture.value().desc.width,
                          color_texture.value().desc.height, 0, 0,
                          config::window_width, config::window_height,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return *this;
    }

    Framebuffer& color(u32 width = config::window_width,
                       u32 height = config::window_height) {
        if (!resource()) {
            PlayGlException("No valid framebuffer found");
        }

        if (color_texture) {
            return *this;
        }

        color_texture.emplace(Texture::from_desc(
            TextureDesc{width, height, TextureDesc::Format::RGBA32F}));
        glBindFramebuffer(GL_FRAMEBUFFER, resource());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, color_texture.value().resource(),
                               0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            PlayGlException("Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return *this;
    }

    Framebuffer& depth(u32 width = config::window_width,
                       u32 height = config::window_height) {
        if (!resource()) {
            PlayGlException("No valid framebuffer found");
        }

        if (depth_texture) {
            return *this;
        }

        depth_texture.emplace(Texture::from_desc(
            TextureDesc{width, height, TextureDesc::Format::Depth32}));
        glBindFramebuffer(GL_FRAMEBUFFER, resource());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depth_texture.value().resource(),
                               0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            PlayGlException("Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return *this;
    }

    void bind() const {
        if (resource()) {
            glBindFramebuffer(GL_FRAMEBUFFER, resource());
            glViewport(0, 0, color_texture.value().desc.width,
                       color_texture.value().desc.height);
        }
    }

    void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    optional<Texture> color_texture;
    optional<Texture> depth_texture;

private:
    virtual u32 create_resource() const override {
        auto result = 0U;
        glGenFramebuffers(1, &result);
        return result;
    }

    static void resource_deleter(u32& resource) {
        if (resource) {
            glDeleteFramebuffers(1, &resource);
        }
    }

    Content& content;
};

class FramebufferContainer {
public:
    FramebufferContainer(Content& content) : content(content) {}

    Framebuffer& operator()(const string& framebuffer_id) {
        return get(framebuffer_id);
    }

private:
    Framebuffer& get(const string& framebuffer_id) {
        if (id_to_framebuffers.count(framebuffer_id)) {
            return id_to_framebuffers.find(framebuffer_id)->second;
        }

        return id_to_framebuffers.insert({framebuffer_id, Framebuffer{content}})
            .first->second;
    }

    Content& content;
    unordered_map<string, Framebuffer> id_to_framebuffers;
};