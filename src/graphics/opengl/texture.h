#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common.h"
#include "resource.h"

class Texture : public LazyResource<u32> {
public:
    static Texture from_file(const Path& path) { return Texture{path}; }

    void bind() const {
        if (resource() && !bound) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, resource());
            bound = true;
        }
    }

    void unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
        bound = false;
    }

private:
    Texture(const Path& path)
        : LazyResource(texture_resource_deleter), path(path) {}

    virtual u32 create_resource() const override {
        u32 resource = 0;
        glGenTextures(1, &resource);
        glBindTexture(GL_TEXTURE_2D, resource);
        // set the texture wrapping/filtering options (on the currently bound
        // texture object)

        // TODO(panmar): Can we change it live (e.g. during shader parameter
        // setup)?
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);
        i32 width, height, channels;
        buffer.reset(
            stbi_load(path.string().c_str(), &width, &height, &channels, 0));
        if (buffer) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, buffer.get());
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            throw std::runtime_error("Failed to load texture");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        return resource;
    }

    static void texture_resource_deleter(u32& resource) {
        if (resource) {
            glDeleteTextures(1, &resource);
            resource = 0;
        }
    }

    static void image_data_deleter(u8* image) { stbi_image_free(image); }

    const Path path;
    mutable unique_ptr<u8, decltype(&image_data_deleter)> buffer{
        nullptr, image_data_deleter};
    mutable bool bound = false;
};
