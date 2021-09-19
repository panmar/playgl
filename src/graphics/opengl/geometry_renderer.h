#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "picosha2.h"

#include "common.h"
#include "graphics/opengl/geometry.h"
#include "graphics/opengl/state.h"
#include "graphics/opengl/shader.h"
#include "camera.h"

struct GpuBuffer {
    GpuBuffer() = default;

    GpuBuffer(GpuBuffer&& other) : vao(other.vao), vbo(other.vbo) {
        other.vao = other.vbo = 0;
    }

    static GpuBuffer from(const Geometry& geometry, const Shader& shader) {
        auto buffer = GpuBuffer{};

        if (!shader.has_attrib(Shader::INPUT_POSITION_ATTRIB)) {
            throw std::runtime_error(
                fmt::format("Attrib {} does not exist in the shader.",
                            Shader::INPUT_POSITION_ATTRIB));
        }

        if (geometry.positions.empty()) {
            throw std::runtime_error("Geometry has no positions.");
        }

        glGenVertexArrays(1, &buffer.vao);
        glBindVertexArray(buffer.vao);

        if (!geometry.indices.empty()) {
            u32 ebo = 0;
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(geometry.indices[0]) * geometry.indices.size(),
                         geometry.indices.data(), GL_STATIC_DRAW);
        }

        // TODO(panmar): Should I disable VertexAttribArray?
        // According below it is not necessary:
        // https://stackoverflow.com/questions/12427880/is-it-important-to-call-gldisablevertexattribarray

        // TODO(panmar): Do I need to store it to release it later?
        u32 vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(geometry.positions[0]) * geometry.positions.size(),
                     geometry.positions.data(), GL_STATIC_DRAW);

        // Shader::INPUT_POSITION_ATTRIB

        {
            auto attrib_location =
                shader.query_attrib_location(Shader::INPUT_POSITION_ATTRIB);
            glEnableVertexAttribArray(attrib_location);
            glVertexAttribPointer(attrib_location, 3, GL_FLOAT, false,
                                  3 * sizeof(f32), (void*)0);
        }

        // Shader::INPUT_NORMAL_ATTRIB
        if (!geometry.normals.empty() &&
            shader.has_attrib(Shader::INPUT_NORMAL_ATTRIB)) {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(vec3) * geometry.normals.size(),
                         geometry.normals.data(), GL_STATIC_DRAW);
            auto attrib_location =
                shader.query_attrib_location(Shader::INPUT_NORMAL_ATTRIB);
            glEnableVertexAttribArray(attrib_location);
            glVertexAttribPointer(attrib_location, 3, GL_FLOAT, false,
                                  3 * sizeof(f32), (void*)0);
        }

        // Shader::INPUT_TEXCOORD_ATTRIB
        if (!geometry.texcoords.empty() &&
            shader.has_attrib(Shader::INPUT_TEXCOORD_ATTRIB)) {
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(vec2) * geometry.texcoords.size(),
                         geometry.texcoords.data(), GL_STATIC_DRAW);
            auto attrib_location =
                shader.query_attrib_location(Shader::INPUT_TEXCOORD_ATTRIB);
            glEnableVertexAttribArray(attrib_location);
            glVertexAttribPointer(attrib_location, 2, GL_FLOAT, false,
                                  2 * sizeof(f32), (void*)0);
        }

        glBindVertexArray(0);

        return buffer;
    }

    ~GpuBuffer() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
        }

        if (vbo) {
            glDeleteBuffers(1, &vbo);
        }
    }

    static string generate_hash(const Geometry& geometry,
                                const Shader& shader) {
        picosha2::hash256_one_by_one hasher;
        static const string separator = "--#!@#!@#--";

        if (!geometry.positions.empty() &&
            shader.has_attrib(Shader::INPUT_POSITION_ATTRIB)) {
            hasher.process(
                reinterpret_cast<const u8*>(&geometry.positions.begin()->x),
                reinterpret_cast<const u8*>(&geometry.positions.end()->x));
        }

        hasher.process(std::begin(separator), std::end(separator));

        if (!geometry.normals.empty() &&
            shader.has_attrib(Shader::INPUT_NORMAL_ATTRIB)) {
            hasher.process(
                reinterpret_cast<const u8*>(&geometry.normals.begin()->x),
                reinterpret_cast<const u8*>(&geometry.normals.end()->x));
        }

        hasher.process(std::begin(separator), std::end(separator));

        if (!geometry.texcoords.empty() &&
            shader.has_attrib(Shader::INPUT_TEXCOORD_ATTRIB)) {
            hasher.process(
                reinterpret_cast<const u8*>(&geometry.texcoords.begin()->x),
                reinterpret_cast<const u8*>(&geometry.texcoords.end()->x));
        }

        auto shader_source = shader.source();
        hasher.process(shader_source.begin(), shader_source.end());
        hasher.finish();

        std::vector<u8> hash(picosha2::k_digest_size);
        hasher.get_hash_bytes(hash.begin(), hash.end());

        std::string hex_str = picosha2::get_hash_hex_string(hasher);

        return hex_str;
    }

    void bind() {
        if (vao) {
            glBindVertexArray(vao);
        }
    }

    void unbind() {}

    u32 vbo = 0;
    u32 vao = 0;
};

class GpuBufferHashmap {
public:
    GpuBuffer& get(const Geometry& geometry, const Shader& shader) {
        auto hash = GpuBuffer::generate_hash(geometry, shader);
        if (hashed_buffers.count(hash)) {
            return hashed_buffers.find(hash)->second;
        } else {
            auto gpu_buffer = GpuBuffer::from(geometry, shader);
            return hashed_buffers.insert({hash, std::move(gpu_buffer)})
                .first->second;
        }
    }

private:
    unordered_map<string, GpuBuffer> hashed_buffers;
};


class GeometryRenderer {
public:
    void render(const Geometry& geometry, const Shader& shader,
                const GpuState& state = GpuState()) const {
        auto& gpu_buffer = hashed_gpubuffers.get(geometry, shader);

        gpu_buffer.bind();
        shader.bind();
        state.bind();

        if (geometry.indices.empty()) {
            glDrawArrays(static_cast<i32>(geometry.topology), 0,
                         geometry.positions.size());
        } else {
            glDrawElements(static_cast<i32>(geometry.topology),
                           geometry.indices.size(), GL_UNSIGNED_INT, 0);
        }

        gpu_buffer.unbind();
        shader.unbind();
        state.unbind();
    }

private:
    mutable GpuBufferHashmap hashed_gpubuffers;
};
