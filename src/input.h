#pragma once

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "common.h"

class Input {
public:
    Input() {
        prev_keys.fill(GLFW_RELEASE);
        keys.fill(GLFW_RELEASE);
    }

    void update() { prev_keys = keys; }

    void on_key_changed(i32 key, i32 scancode, i32 action, i32 mods) {
        keys[key] = action == GLFW_RELEASE ? GLFW_RELEASE : GLFW_PRESS;
    }

    bool is_key_down(i32 key) const {
        if (key >= keys.size()) {
            return false;
        }
        return keys[key] == GLFW_PRESS;
    }

    bool is_key_pressed(i32 key) const {
        if (key >= keys.size()) {
            return false;
        }
        return keys[key] == GLFW_PRESS && prev_keys[key] == GLFW_RELEASE;
    }

private:
    array<i32, GLFW_KEY_LAST + 1> keys;
    array<i32, GLFW_KEY_LAST + 1> prev_keys;
};
