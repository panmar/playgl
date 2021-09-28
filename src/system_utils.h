#pragma once

#include <fstream>
#include <random>

#include "common.h"

inline string read_file(const std::filesystem::path& filepath) {
    if (!std::filesystem::exists(filepath)) {
        throw PlayGlException(
            fmt::format("File does not exist: `{}`", filepath.string()));
    }

    std::ifstream ifs(filepath);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    return content;
}

inline f32 random(f32 min, f32 max) {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<f32> distr(min, max);
    return distr(eng);
}