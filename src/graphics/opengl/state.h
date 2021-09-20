#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

class GpuState {
public:
    enum class CullFaceMode {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontAndBack = GL_FRONT_AND_BACK
    };

    GpuState& nodepth() {
        _depth_test = false;
        return *this;
    }

    GpuState& cull_face(CullFaceMode mode = CullFaceMode::Back) {
        _cull_face = true;
        _cull_face_mode = mode;
        return *this;
    }

    void bind() const {
        if (bind_lock) {
            std::runtime_error(
                "Cannot bind GpuState. Unbind previous state first.");
        }

        bind_lock = true;

        if (_depth_test) {
            glEnable(GL_DEPTH_TEST);
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
            std::runtime_error("GpuState has already been unbound.");
        }

        bind_lock = false;
    }

private:
    mutable bool bind_lock = false;

    bool _depth_test = true;
    bool _cull_face = false;
    CullFaceMode _cull_face_mode = CullFaceMode::Back;
};