#pragma once

#include <variant>

#include "common.h"

class Store {
public:
private:
    using StoreValue = std::variant<i32, f32>;
    std::map<std::string, StoreValue> keyValues;
};
