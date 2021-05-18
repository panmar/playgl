#pragma once

#include <variant>

#include "common.h"

class Store {
public:
    template <class T>
    void set(const string& key, const T& value) {
        key_values.insert_or_assign(value);
    }

    template <class T>
    T get(const string& name) {
        auto it = key_values.find(name);
        if (it == key_values.end()) {
        }
        auto& value = it->second;
        return std::get<T>(value);
    }

private:
    using Value = std::variant<i32, f32>;
    std::map<std::string, Value> key_values;
};
