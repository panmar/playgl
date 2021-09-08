#pragma once

#include <array>
#include <assert.h>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

using std::array;
using std::map;
using std::unordered_map;
using std::set;
using std::string;
using std::vector;

using std::unique_ptr;

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

#include "mathgl.h"
