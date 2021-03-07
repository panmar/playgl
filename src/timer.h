#pragma once

#include <chrono>

#include "common.h"

class Timer {
public:
    void tick() {
        auto tick = std::chrono::high_resolution_clock::now();
        std::chrono::duration<f32> diff = tick - prev_tick;
        elapsed_seconds = diff.count();
        prev_tick = tick;
    }

    f32 get_elapsed_seconds() const { return elapsed_seconds; }

private:
    std::chrono::high_resolution_clock::time_point prev_tick =
        std::chrono::high_resolution_clock::now();
    f32 elapsed_seconds = 0.f;
};
