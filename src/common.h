#pragma once

#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>

using std::array;
using std::map;
using std::unordered_map;
using std::unordered_set;
using std::set;
using std::string;
using std::vector;

using std::unique_ptr;
using Path = std::filesystem::path;
using std::optional;

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

#include "error.h"
#include "system_utils.h"
#include "mathgl.h"
