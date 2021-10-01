#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "common.h"
#include "resource.h"

struct TextureDesc {
    enum class Format { RGBA8 = GL_RGBA8, RGBA32F = GL_RGBA32F, Depth32 };
    enum class Filter { Linear = GL_LINEAR };

    u32 width = 0;
    u32 height = 0;
    Format format = Format::RGBA8;
    Filter min_filter = Filter::Linear;
    Filter max_filter = Filter::Linear;

    f32 aspect_ratio() const {
        return static_cast<f32>(width) / height;
    }
};

class Texture : public LazyResource<u32> {
public:
    static Texture from_file(const Path& path) { return Texture{path}; }

    static Texture from_desc(const TextureDesc& desc) { return Texture{desc}; }

    void bind(u32 slot = 0) const {
        if (resource()) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, resource());
        }
    }

    void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    // NOTE(panmar): If the texture was created from desc it is stored here
    mutable TextureDesc desc;

private:
    Texture(const Path& path)
        : LazyResource(texture_resource_deleter), path(path) {}

    Texture(const TextureDesc& desc)
        : LazyResource(texture_resource_deleter), desc(desc) {}

    virtual u32 create_resource() const override {
        if (path.empty()) {
            return create_texture_from_desc(desc);
        }
        u32 resource = create_texture_from_path(path);

        i32 width = 0, height = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        desc.width = width;
        desc.height = height;

        return resource;
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
        auto format = (channels == 3) ? GL_SRGB : GL_SRGB_ALPHA;
        if (buffer) {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB,
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
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, desc.width,
                           desc.height);
        } else {
            glTexStorage2D(GL_TEXTURE_2D, 1, static_cast<i32>(desc.format),
                           desc.width, desc.height);
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
