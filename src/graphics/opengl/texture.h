#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common.h"
#include "resource.h"

struct TextureDesc {
    enum class Format { RGB = GL_RGB, Depth32 };
    enum class Filter { Linear = GL_LINEAR };
    u32 width = 0;
    u32 height = 0;
    Format format = Format::RGB;
    Filter min_filter = Filter::Linear;
    Filter max_filter = Filter::Linear;
};

class Texture : public LazyResource<u32> {
public:
    static Texture from_file(const Path& path) { return Texture{path}; }

    static Texture from_desc(const TextureDesc& desc) { return Texture{desc}; }

    void bind() const {
        if (resource()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, resource());
        }
    }

    void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    // NOTE(panmar): If the texture was created from desc it is stored here
    const TextureDesc desc;

private:
    Texture(const Path& path)
        : LazyResource(texture_resource_deleter), path(path) {}

    Texture(const TextureDesc& desc)
        : LazyResource(texture_resource_deleter), desc(desc) {}

    virtual u32 create_resource() const override {
        if (path.empty()) {
            return create_texture_from_desc(desc);
        }
        return create_texture_from_path(path);
    }

    static u32 create_texture_from_path(const Path& path) {
        u32 texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);
        i32 width, height, channels;
        unique_ptr<u8, decltype(&image_data_deleter)> buffer{
            stbi_load(path.string().c_str(), &width, &height, &channels, 0),
            image_data_deleter};
        if (buffer) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, buffer.get());
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            throw PlayGlException("Failed to load texture");
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }

    static u32 create_texture_from_desc(const TextureDesc& desc) {
        u32 texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        if (desc.format == TextureDesc::Format::Depth32) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, desc.width,
                         desc.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<i32>(desc.format),
                         desc.width, desc.height, 0,
                         static_cast<i32>(desc.format), GL_UNSIGNED_BYTE, NULL);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        static_cast<i32>(desc.min_filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        static_cast<i32>(desc.max_filter));
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

    static void texture_resource_deleter(u32& resource) {
        if (resource) {
            glDeleteTextures(1, &resource);
            resource = 0;
        }
    }

    static void image_data_deleter(u8* image) { stbi_image_free(image); }

    // NOTE(panmar): If the texture was created from path it is the path
    const Path path;
};
