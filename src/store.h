#pragma once

#include "common.h"

template <class T>
struct BoundedParam {
    BoundedParam(const T& value, f32 min, f32 max)
        : value(value), min(min), max(max) {}

    T value;
    f32 min;
    f32 max;
};

template <>
struct BoundedParam<Color> {
    BoundedParam(const Color& value, f32 min, f32 max)
        : value(value), min(min), max(max) {
        throw PlayGlException("Bounded color not supported.");
    }

    Color value;
    f32 min;
    f32 max;
};

class StoreParam {
public:
    enum AnnotationType { Gui = 0b01, Shader = 0b10, Bounded = 0b100 };

    operator i32&() { return std::get<i32>(param); }
    operator f32&() { return std::get<f32>(param); }
    operator vec3&() { return std::get<vec3>(param); }
    operator vec4&() { return std::get<vec4>(param); }
    operator mat4&() { return std::get<mat4>(param); }
    operator Color&() { return std::get<Color>(param); }
    operator string&() { return std::get<string>(param); }
    operator const char*() { return std::get<string>(param).c_str(); }

    operator const i32&() const { return std::get<i32>(param); }
    operator const f32&() const { return std::get<f32>(param); }
    operator const vec3&() const { return std::get<vec3>(param); }
    operator const vec4&() const { return std::get<vec4>(param); }
    operator const mat4&() const { return std::get<mat4>(param); }
    operator const Color&() const { return std::get<Color>(param); }
    operator const string&() const { return std::get<string>(param); }

    template <typename T>
    StoreParam& operator=(const T& param) {
        this->param = param;
        return *this;
    }

    template <typename T>
    StoreParam& operator=(const BoundedParam<T>& bounded_param) {
        this->param = bounded_param.value;
        this->annotations |= AnnotationType::Bounded;
        this->min_bound = bounded_param.min;
        this->max_bound = bounded_param.max;
        return *this;
    }

    // NOTE(panmar): To avoid confusion and reduce number of errors
    // we do store unsigned integers as signed integers;
    template <>
    StoreParam& operator=(const u32& param) {
        this->param = static_cast<i32>(param);
        return *this;
    }

    bool has(u32 annotation_flags) { return annotations & annotation_flags; }

    using ParamType =
        std::variant<i32, f32, glm::vec3, glm::vec4, glm::mat4, Color, string>;

    ParamType param;
    u32 annotations = AnnotationType::Gui | AnnotationType::Shader;
    f32 min_bound = 0.f;
    f32 max_bound = 0.f;
};

class Store {
public:
    StoreParam& operator[](const string& name) {
        auto it = named_params.find(name);
        if (it == named_params.end()) {
            it = named_params.insert({name, StoreParam()}).first;
        }
        return it->second;
    }

    const StoreParam& operator[](const string& name) const {
        auto it = named_params.find(name);
        return it->second;
    }

    auto begin() { return named_params.begin(); }

    auto end() { return named_params.end(); }

private:
    std::unordered_map<std::string, StoreParam> named_params;
};
