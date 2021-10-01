#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"
#include "graphics/texture.h"
#include "resource.h"

class Shader : public LazyResource<u32> {
public:
    static constexpr const char* INPUT_POSITION_ATTRIB = "IN_POSITION";
    static constexpr const char* INPUT_NORMAL_ATTRIB = "IN_NORMAL";
    static constexpr const char* INPUT_TEXCOORD_ATTRIB = "IN_TEXCOORD";

    static Shader from_text(const string& vs_text, const string& fs_text) {
        return Shader(vs_text, fs_text);
    }

    static Shader from_file(const Path& vs_path, const Path& fs_path) {
        return Shader(vs_path, fs_path);
    }

    bool has_attrib(const char* name) const {
        return glGetAttribLocation(resource(), name) != -1;
    }

    i32 query_attrib_location(const char* name) const {
        return glGetAttribLocation(resource(), name);
    }

    template <class ParamType>
    const Shader& try_param(const char* name, const ParamType& value) const {
        try {
            return param(name, value);
        } catch (const PlayGlException& ex) {
            return *this;
        }
    }

    const Shader& param(const char* name, bool value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniform1i(location, static_cast<i32>(value));
        return *this;
    }

    const Shader& param(const char* name, i32 value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniform1i(location, value);
        return *this;
    }

    const Shader& param(const char* name, f32 value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniform1f(location, value);
        return *this;
    }

    const Shader& param(const char* name, const glm::vec2& value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniform2fv(location, 1, &value[0]);
        return *this;
    }

    const Shader& param(const char* name, const glm::vec3& value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniform3fv(location, 1, &value[0]);
        return *this;
    }

    const Shader& param(const char* name, const glm::vec4& value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniform4fv(location, 1, &value[0]);
        return *this;
    }

    const Shader& param(const char* name, const Color& value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniform4fv(location, 1, value.data);
        return *this;
    }

    const Shader& param(const char* name, const glm::mat2& value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniformMatrix2fv(location, 1, GL_TRUE, &value[0][0]);
        return *this;
    }

    const Shader& param(const char* name, const glm::mat3& value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniformMatrix3fv(location, 1, GL_TRUE, &value[0][0]);
        return *this;
    }

    const Shader& param(const char* name, const glm::mat4& value) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }
        bind();
        glUniformMatrix4fv(location, 1, GL_TRUE, &value[0][0]);
        return *this;
    }

    const Shader& param(const char* name, const Texture& texture) const {
        auto location = glGetUniformLocation(resource(), name);
        if (location == -1) {
            throw PlayGlException(
                fmt::format("Shader[{}:{}]: Could not find param `{}`",
                            vs_path.filename().string(),
                            fs_path.filename().string(), name));
        }

        constexpr u32 MAX_SAMPELRS = 8;
        if (current_sampler_slot > MAX_SAMPELRS) {
            throw PlayGlException("Number of samplers exceeded");
        }

        bind();
        texture.bind(current_sampler_slot);
        param(name, static_cast<i32>(current_sampler_slot));
        ++current_sampler_slot;
        return *this;
    }

    void bind() const {
        if (resource() && !bound) {
            glUseProgram(resource());
            current_sampler_slot = 0;
            bound = true;
        }
    }

    void unbind() const {
        // glUseProgram(0);
        bound = false;
    }

    string source() const {
        resource();
        // TODO(panmar): Could be cached
        return vs_text + fs_text;
    }

private:
    Shader(const string& vs_text, const string& fs_text)
        : LazyResource(shader_resource_deleter),
          vs_text(vs_text),
          fs_text(fs_text) {}

    Shader(const Path& vs_path, const Path& fs_path)
        : LazyResource(shader_resource_deleter),
          vs_path(vs_path),
          fs_path(fs_path) {}

    virtual u32 create_resource() const override {
        if (!vs_path.empty()) {
            vs_text = read_file(vs_path);
        }

        if (!fs_path.empty()) {
            fs_text = read_file(fs_path);
        }

        auto vs = glCreateShader(GL_VERTEX_SHADER);
        const char* vs_cstr = vs_text.c_str();
        glShaderSource(vs, 1, &vs_cstr, nullptr);
        glCompileShader(vs);
        log_shader_errors_if_any(vs);

        auto fs = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fs_cstr = fs_text.c_str();
        glShaderSource(fs, 1, &fs_cstr, nullptr);
        glCompileShader(fs);
        log_shader_errors_if_any(fs);

        u32 program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        log_program_errors_if_any(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        auto compute_label = [](const Path& vs_path,
                                const Path& fs_path) -> string {
            if (vs_path.empty() || fs_path.empty()) {
                return "";
            }

            if (vs_path.stem() == fs_path.stem()) {
                return vs_path.stem().string();
            }

            return fmt::format("{}:{}", vs_path.stem().string(),
                               fs_path.stem().string());
        };

        debug::label(compute_label(vs_path, fs_path), GL_PROGRAM, program);

        return program;
    };

    static void shader_resource_deleter(u32& resource) {
        if (resource) {
            glDeleteProgram(resource);
            resource = 0;
        }
    }

    bool log_shader_errors_if_any(u32 shader) const {
        char message[1024];
        i32 success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, message);
            printf("Shader compilation error:\n");
            printf("%s\n", message);
        }
        return success != 0;
    }

    bool log_program_errors_if_any(u32 program) const {
        char message[1024];
        i32 success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 1024, nullptr, message);
            printf("Program compilation error:\n");
            printf("%s\n", message);
        }
        return success != 0;
    }

    const Path vs_path;
    const Path fs_path;
    mutable string vs_text;
    mutable string fs_text;

    mutable bool bound = false;
    mutable u32 current_sampler_slot = 0;
};