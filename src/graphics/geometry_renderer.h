#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "meow_hash.h"

#include "common.h"
#include "graphics/geometry.h"
#include "graphics/state.h"
#include "graphics/shader.h"
#include "store.h"

struct GpuBuffer {
    GpuBuffer() = default;

    GpuBuffer(GpuBuffer&& other) : vao(other.vao), vbo(other.vbo) {
        other.vao = other.vbo = 0;
    }

    static GpuBuffer from(const Geometry& geometry, const Shader& shader) {
        auto buffer = GpuBuffer{};

        if (!shader.has_attrib(Shader::INPUT_POSITION_ATTRIB)) {
            throw PlayGlException(
                fmt::format("Attrib {} does not exist in the shader.",
                            Shader::INPUT_POSITION_ATTRIB));
        }

        if (geometry.positions.empty()) {
            throw PlayGlException("Geometry has no positions.");
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

    static u64 generate_hash(const Geometry& geometry, const Shader& shader) {
        static const string separator = "--#!@#!@#--";
        meow_state state;
        MeowBegin(&state, MeowDefaultSeed);

        if (!geometry.positions.empty() &&
            shader.has_attrib(Shader::INPUT_POSITION_ATTRIB)) {
            MeowAbsorb(
                &state,
                geometry.positions.size() * sizeof(geometry.positions[0]),
                (void*)(&geometry.positions.begin()->x));
        }

        MeowAbsorb(&state, separator.size() * sizeof(std::string::value_type),
                   (void*)(separator.c_str()));

        if (!geometry.normals.empty() &&
            shader.has_attrib(Shader::INPUT_NORMAL_ATTRIB)) {
            MeowAbsorb(&state,
                       geometry.normals.size() * sizeof(geometry.normals[0]),
                       (void*)(&geometry.normals.begin()->x));
        }

        MeowAbsorb(&state, separator.size() * sizeof(std::string::value_type),
                   (void*)(separator.c_str()));

        if (!geometry.texcoords.empty() &&
            shader.has_attrib(Shader::INPUT_TEXCOORD_ATTRIB)) {
            MeowAbsorb(
                &state,
                geometry.texcoords.size() * sizeof(geometry.texcoords[0]),
                (void*)(&geometry.texcoords.begin()->x));
        }

        auto shader_source = shader.source();
        MeowAbsorb(&state,
                   shader_source.size() * sizeof(std::string::value_type),
                   (void*)(shader_source.c_str()));

        auto hash128 = MeowEnd(&state, nullptr);
        auto hash64 = MeowU64From(hash128, 0);
        return hash64;
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
            // NOTE(panmar): Safety check - do our hashing works correctly?
            constexpr auto MAX_HASHED_BUFFERS = 50;
            if (hashed_buffers.size() > MAX_HASHED_BUFFERS) {
                throw PlayGlException(
                    "Maximum number of hashed buffers exceeded");
            }

            auto gpu_buffer = GpuBuffer::from(geometry, shader);
            return hashed_buffers.insert({hash, std::move(gpu_buffer)})
                .first->second;
        }
    }

private:
    unordered_map<u64, GpuBuffer> hashed_buffers;
};

class GeometryRendererCommand {
public:
    GeometryRendererCommand(Content& content, Store& store,
                            GpuBufferHashmap& hashed_gpubuffers,
                            const Geometry& geometry)
        : content(content),
          store(store),
          hashed_gpubuffers(hashed_gpubuffers),
          _geometry_ref(&geometry) {
        debug::scope_start("geometry:render");
    }

    GeometryRendererCommand(Content& content, Store& store,
                            GpuBufferHashmap& hashed_gpubuffers,
                            Geometry&& geometry)
        : content(content),
          store(store),
          hashed_gpubuffers(hashed_gpubuffers),
          _geometry(std::move(geometry)) {
        debug::scope_start("geometry:render");
    }

    // NOTE(panmar): To support those operation we should add proper handling of
    // debug-scope transfer
    GeometryRendererCommand(const GeometryRendererCommand&) = delete;
    GeometryRendererCommand(GeometryRendererCommand&&) = delete;
    GeometryRendererCommand& operator=(const GeometryRendererCommand&) = delete;
    GeometryRendererCommand& operator=(GeometryRendererCommand&&) = delete;

    ~GeometryRendererCommand() { debug::scope_end(); }

    GeometryRendererCommand& shader(const string& vs_id, const string& fs_id) {
        _shader = &content.shader(vs_id, fs_id);
        return *this;
    }

    GeometryRendererCommand& shader(Shader& shader_program) {
        _shader = &shader_program;
        return *this;
    }

    template <class ParamType>
    GeometryRendererCommand& param(const char* name, const ParamType& param) {
        if (!_shader) {
            throw PlayGlException("Shader not set");
        }
        _shader->param(name, param);
        return *this;
    }

    GeometryRendererCommand& state(const GpuState& gpu_state) {
        _state = gpu_state;
        return *this;
    }

    void render() {
        auto& geometry = get_geometry();

        if (!_shader) {
            throw PlayGlException("Shader not set");
        }

        auto& gpu_buffer = hashed_gpubuffers.get(geometry, *_shader);
        gpu_buffer.bind();

        _shader->bind();
        populate_shader_params_from_store(*_shader);
        _state.bind();

        if (geometry.indices.empty()) {
            glDrawArrays(static_cast<i32>(geometry.topology), 0,
                         geometry.positions.size());
        } else {
            glDrawElements(static_cast<i32>(geometry.topology),
                           geometry.indices.size(), GL_UNSIGNED_INT, 0);
        }

        gpu_buffer.unbind();
        _shader->unbind();
        _state.unbind();
    }

private:
    void populate_shader_params_from_store(const Shader& shader) {
        for (auto& key_value : store) {
            auto& name = key_value.first;
            auto& param = key_value.second;
            if (param.has(StoreParam::Shader)) {
                std::visit(
                    [&name, &shader](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, i32>) {
                            shader.try_param(name.c_str(), arg);
                        } else if constexpr (std::is_same_v<T, f32>) {
                            shader.try_param(name.c_str(), arg);
                        } else if constexpr (std::is_same_v<T, vec2>) {
                            shader.try_param(name.c_str(), arg);
                        } else if constexpr (std::is_same_v<T, vec3>) {
                            shader.try_param(name.c_str(), arg);
                        } else if constexpr (std::is_same_v<T, vec4>) {
                            shader.try_param(name.c_str(), arg);
                        } else if constexpr (std::is_same_v<T, mat4>) {
                            shader.try_param(name.c_str(), arg);
                        } else if constexpr (std::is_same_v<T, Color>) {
                            shader.try_param(name.c_str(), arg);
                        } else if constexpr (std::is_same_v<T, string>) {
                            // NOTE(panmar): Not supported
                        } else {
                            static_assert(false);
                        }
                    },
                    param.param);
            }
        }
    }

    const Geometry& get_geometry() {
        if (_geometry_ref) {
            return *_geometry_ref;
        }
        return _geometry;
    }

    Content& content;
    Store& store;
    GpuBufferHashmap& hashed_gpubuffers;

    const Geometry* _geometry_ref = nullptr;
    Geometry _geometry;

    Shader* _shader = nullptr;

    GpuState _state;
};

class GeometryRenderer {
public:
    GeometryRenderer(Content& content, Store& store)
        : content(content), store(store) {}

    GeometryRendererCommand operator()(const Geometry& _geometry) {
        return command(_geometry);
    }

    GeometryRendererCommand operator()(Geometry&& _geometry) {
        return command(std::move(_geometry));
    }

private:
    GeometryRendererCommand command(const Geometry& geometry) {
        return GeometryRendererCommand(content, store, hashed_gpubuffers,
                                       geometry);
    }

    GeometryRendererCommand command(Geometry&& geometry) {
        return GeometryRendererCommand(content, store, hashed_gpubuffers,
                                       std::move(geometry));
    }

    Content& content;
    Store& store;
    GpuBufferHashmap hashed_gpubuffers;
};
