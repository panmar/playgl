#pragma once

#include <fstream>

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