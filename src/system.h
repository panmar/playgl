#pragma once

#include <fstream>

#include "common.h"

inline string read_file(const string& filepath) {
    auto file = std::unique_ptr<FILE, int (*)(FILE*)>{
        fopen(filepath.c_str(), "r"), &fclose};

    if (!file) {
        throw std::runtime_error("File " + filepath + "could not be opened");
    }

    fseek(file.get(), 0, SEEK_END);
    size_t size = ftell(file.get());
    rewind(file.get());

    unique_ptr<char[]> buffer{new char[size]};
    fread(buffer.get(), sizeof(char), size, file.get());

    return string{buffer.get()};
}