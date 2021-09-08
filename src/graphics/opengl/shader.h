#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"

class Shader {
public:
    Shader(const string& vs_str, const string& fs_str) {
        auto vs = glCreateShader(GL_VERTEX_SHADER);
        const char* vs_cstr = vs_str.c_str();
        glShaderSource(vs, 1, &vs_cstr, nullptr);
        glCompileShader(vs);
        log_shader_errors_if_any(vs);

        auto fs = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fs_cstr = fs_str.c_str();
        glShaderSource(fs, 1, &fs_cstr, nullptr);
        glCompileShader(fs);
        log_shader_errors_if_any(fs);

        this->program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        log_program_errors_if_any(program);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    ~Shader() {
        if (program) {
            glDeleteProgram(program);
        }
    }

    bool log_shader_errors_if_any(u32 shader) {
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

    bool log_program_errors_if_any(u32 program) {
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

    bool set_param(const char* name, bool value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform1i(location, static_cast<i32>(value));
        return true;
    }

    bool set_param(const char* name, i32 value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform1i(location, value);
        return true;
    }

    bool set_param(const char* name, f32 value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform1f(location, value);
        return true;
    }

    bool set_param(const char* name, const glm::vec2& value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform2fv(location, 1, &value[0]);
        return true;
    }

    bool set_param(const char* name, f32 x, f32 y) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform2f(location, x, y);
        return true;
    }

    bool set_param(const char* name, const glm::vec3& value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform3fv(location, 1, &value[0]);
        return true;
    }

    bool set_param(const char* name, f32 x, f32 y, f32 z) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform3f(location, x, y, z);
        return true;
    }

    bool set_param(const char* name, const glm::vec4& value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform4fv(location, 1, &value[0]);
        return true;
    }

    bool set_param(const char* name, f32 x, f32 y, f32 z, f32 w) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniform4f(location, x, y, z, w);
        return true;
    }

    bool set_param(const char* name, const glm::mat2& value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
        return true;
    }

    bool set_param(const char* name, const glm::mat3& value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
        return true;
    }

    bool set_param(const char* name, const glm::mat4& value) const {
        auto location = glGetUniformLocation(program, name);
        if (location == -1) {
            return false;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        return true;
    }

    void bind() {
        assert(program != 0);
        glUseProgram(program);
    }

private:
    u32 program = 0;
};