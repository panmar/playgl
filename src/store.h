#pragma once

#include <variant>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "common.h"

class Store {
public:
    template <class T>
    void set(const string& key, const T& value) {
        key_values.insert_or_assign(key, value);
    }

    template <class T>
    T get(const string& name) const {
        auto it = key_values.find(name);
        if (it == key_values.end()) {
        }
        auto& value = it->second;
        return std::get<T>(value);
    }

private:
    using Value = std::variant<i32, u32, f32, glm::vec3, glm::mat4>;
    std::map<std::string, Value> key_values;
};
