#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"
#include "config.h"

class GpuStateCache {
public:
    static void clear() {
        enabled_flags().clear();
        cache().clear();
    }

    static void glEnable(u32 flag) {
        if (!enabled_flags().count(flag)) {
            ::glEnable(flag);
            enabled_flags().insert(flag);
        }
    }

    static void glDisable(u32 flag) {
        if (enabled_flags().count(flag)) {
            ::glDisable(flag);
            enabled_flags().erase(flag);
        }
    }

    static void glClipControl(u32 arg1, u32 arg2) {
        if (!check_and_set(CLIP_CONTROL, arg1, arg2)) {
            ::glClipControl(arg1, arg2);
        }
    }

    static void glBlendFunc(u32 arg1, u32 arg2) {
        if (!check_and_set(BLEND_FUNC, arg1, arg2)) {
            ::glBlendFunc(arg1, arg2);
        }
    }

    static void glBlendEquation(u32 arg1) {
        if (!check_and_set(BLEND_EQUATION, arg1)) {
            ::glBlendEquation(arg1);
        }
    }

    static void glDepthMask(u32 arg1) {
        if (!check_and_set(DEPTH_MASK, arg1)) {
            ::glDepthMask(arg1);
        }
    }

    static void glDepthFunc(u32 arg1) {
        if (!check_and_set(DEPTH_FUNC, arg1)) {
            ::glDepthFunc(arg1);
        }
    }

    static void glCullFace(u32 arg1) {
        if (!check_and_set(CULL_FACE, arg1)) {
            ::glCullFace(arg1);
        }
    }

    static void glPolygonMode(u32 arg1, u32 arg2) {
        if (!check_and_set(POLYGON_MODE, arg1, arg2)) {
            ::glPolygonMode(arg1, arg2);
        }
    }

    static void glBlendColor(f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
        if (!check_and_set(POLYGON_MODE, arg1, arg2)) {
            ::glBlendColor(arg1, arg2, arg3, arg4);
        }
    }

private:
    enum FuncType {
        ENABLE,
        CLIP_CONTROL,
        BLEND_FUNC,
        BLEND_EQUATION,
        DEPTH_MASK,
        DEPTH_FUNC,
        CULL_FACE,
        POLYGON_MODE
    };

    static unordered_set<u32>& enabled_flags() {
        static unordered_set<u32> _enabled_flags;
        return _enabled_flags;
    }

    using ArgType = std::variant<u32, f32>;
    static unordered_map<u32, vector<ArgType>>& cache() {
        static unordered_map<u32, vector<ArgType>> _cache;
        return _cache;
    };

    template <class... ParamType>
    static bool check_and_set(FuncType func_id, ParamType... params) {
        vector<ArgType> p{params...};

        auto it = cache().find(func_id);
        if (it == cache().end()) {
            cache().insert({func_id, p});
            return false;
        }

        if (std::equal(it->second.begin(), it->second.end(), p.begin(),
                       p.end())) {
            return true;
        }

        cache().erase(func_id);
        cache().insert({func_id, p});
        return false;
    }
};

class GpuState {
public:
    enum class BlendMode {
        Zero = GL_ZERO,
        One = GL_ONE,
        SrcColor = GL_SRC_COLOR,
        OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
        DstColor = GL_DST_COLOR,
        OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DstAlpha = GL_DST_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor = GL_CONSTANT_COLOR,
        OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
        SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE,
        Src1_Color = GL_SRC1_COLOR,
        OneMinusSrc1Color = GL_ONE_MINUS_SRC1_COLOR,
        Src1Alpha = GL_SRC1_ALPHA,
        OneMinusSrc1Alpha = GL_ONE_MINUS_SRC1_ALPHA
    };

    enum class BlendEquation {
        Add = GL_FUNC_ADD,
        Subtract = GL_FUNC_SUBTRACT,
        ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
        Min = GL_MIN,
        Max = GL_MAX
    };

    enum class CullFaceMode {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontAndBack = GL_FRONT_AND_BACK
    };

    GpuState& blend(BlendMode src = BlendMode::One,
                    BlendMode dst = BlendMode::Zero,
                    BlendEquation eq = BlendEquation::Add,
                    Color color = {0.f, 0.f, 0.f, 0.f}) {
        _blend = true;
        _src_blend_mode = src;
        _dst_blend_mode = dst;
        _blend_equation = eq;
        _blend_color = color;
        return *this;
    }

    GpuState& nodepth() {
        _depth_test = false;
        return *this;
    }

    GpuState& nodepth_write() {
        _depth_write = false;
        return *this;
    }

    GpuState& nocull() {
        _cull_face = false;
        return *this;
    }

    GpuState& wireframe() {
        _wireframe = true;
        return *this;
    }

    void bind() const {
        bind_lock = true;

        if (config::multisampling) {
            GpuStateCache::glEnable(GL_MULTISAMPLE);
        }

        if (config::inverse_depth) {
            GpuStateCache::glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
        }

        if (_blend) {
            GpuStateCache::glEnable(GL_BLEND);
            GpuStateCache::glBlendFunc(static_cast<u32>(_src_blend_mode),
                                       static_cast<u32>(_dst_blend_mode));
            GpuStateCache::glBlendEquation(static_cast<u32>(_blend_equation));
            GpuStateCache::glBlendColor(_blend_color.r, _blend_color.g,
                                        _blend_color.b, _blend_color.a);
        } else {
            GpuStateCache::glDisable(GL_BLEND);
        }

        if (_depth_test) {
            GpuStateCache::glEnable(GL_DEPTH_TEST);
            GpuStateCache::glDepthMask(_depth_write);

            if (config::inverse_depth) {
                GpuStateCache::glDepthFunc(GL_GREATER);
            } else {
                GpuStateCache::glDepthFunc(GL_LESS);
            }
        } else {
            GpuStateCache::glDisable(GL_DEPTH_TEST);
        }

        if (_cull_face) {
            GpuStateCache::glEnable(GL_CULL_FACE);
            GpuStateCache::glCullFace(static_cast<i32>(_cull_face_mode));
        } else {
            GpuStateCache::glDisable(GL_CULL_FACE);
        }

        if (_wireframe) {
            GpuStateCache::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            GpuStateCache::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    void unbind() const {
        if (!bind_lock) {
            PlayGlException("GpuState has already been unbound.");
        }

        bind_lock = false;
    }

private:
    mutable bool bind_lock = false;

    bool _blend = false;
    BlendMode _src_blend_mode = BlendMode::One;
    BlendMode _dst_blend_mode = BlendMode::Zero;
    BlendEquation _blend_equation = BlendEquation::Add;
    Color _blend_color = {0.f, 0.f, 0.f, 0.f};

    bool _depth_test = true;
    bool _depth_write = true;

    bool _cull_face = true;
    CullFaceMode _cull_face_mode = CullFaceMode::Back;

    bool _wireframe = false;
};