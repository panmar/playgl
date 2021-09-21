#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"

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

    GpuState& nodepth_write() { _depth_write = false; }

    GpuState& nocull() {
        _cull_face = false;
        return *this;
    }

    void bind() const {
        bind_lock = true;

        if (_blend) {
            glEnable(GL_BLEND);
            glBlendFunc(static_cast<i32>(_src_blend_mode),
                        static_cast<i32>(_dst_blend_mode));
            glBlendEquation(static_cast<i32>(_blend_equation));
            glBlendColor(_blend_color.r, _blend_color.g, _blend_color.b,
                         _blend_color.a);
        } else {
            glDisable(GL_BLEND);
        }

        if (_depth_test) {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(_depth_write);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        if (_cull_face) {
            glEnable(GL_CULL_FACE);
            glCullFace(static_cast<i32>(_cull_face_mode));
        } else {
            glDisable(GL_CULL_FACE);
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
};