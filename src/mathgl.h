#pragma once

#include "common.h"

// NOTE(panmar): Lets use glm for now
// In future it would be nice to have own math implementation

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/compatibility.hpp>

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

struct vec4u8 {
    union {
        struct {
            u8 x, y, z, w;
        };
        u8 v[4];
    };
};

using Color = glm::vec4;

namespace Colors {
    const Color Black = Color{0.f, 0.f, 0.f, 1.f};
    const Color White = Color{1.f, 1.f, 1.f, 1.f};
    const Color Red = Color{1.f, 0.f, 0.f, 1.f};
    const Color Green = Color{0.f, 1.f, 0.f, 1.f};
    const Color Blue = Color{0.f, 0.f, 1.f, 1.f};
}