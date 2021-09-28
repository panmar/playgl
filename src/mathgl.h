#pragma once

#include "common.h"

// NOTE(panmar): Lets use glm for now
// In future it would be nice to have own math implementation

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SINGLE_ONLY
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_XYZW_ONLY
//#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <glm/gtx/rotate_normalized_axis.hpp>

using glm::mat2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

struct Color {
    Color(f32 r, f32 g, f32 b, f32 a = 1.f) : r(r), g(g), b(b), a(a) {}
    union {
        struct {
            f32 r;
            f32 g;
            f32 b;
            f32 a;
        };
        f32 data[4];
    };
};

namespace Colors {

const Color Black = Color{0.f, 0.f, 0.f, 1.f};
const Color White = Color{1.f, 1.f, 1.f, 1.f};
const Color Red = Color{1.f, 0.f, 0.f, 1.f};
const Color Green = Color{0.f, 1.f, 0.f, 1.f};
const Color Blue = Color{0.f, 0.f, 1.f, 1.f};

}  // namespace Colors

inline mat4 compute_inverse_depth_inf_perspective(f32 fov, f32 aspect_ratio,
                                                  f32 _near) {
    // https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
    // https://dev.theomader.com/depth-precision/
    float f = 1.0f / tan(fov / 2.0f);
    return mat4(f / aspect_ratio, 0.0f, 0.0f, 0.0f, 0.0f, f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f, 0.0f, _near, 0.0f);
}