#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

using std::array;
using std::map;
using std::set;
using std::string;
using std::vector;

using i8 = int8_t;
using u8 = uint8_t;
using u32 = uint32_t;
using i32 = int32_t;
using f32 = float;
using f64 = double;

// NOTE(panmar): Lets use glm for now
// In future it would be nice to have own math implementation

#include "glm/ext.hpp"

using glm::mat4;
using glm::vec3;
