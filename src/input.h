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

    void update() {
        prev_keys = keys;
        prev_mouse = mouse;
        scroll_x_offsets.clear();
        scroll_y_offsets.clear();
        prev_cursor_pos_x = cursor_pos_x;
        prev_cursor_pos_y = cursor_pos_y;
    }

    void on_key_changed(i32 key, i32 scancode, i32 action, i32 mods) {
        // NOTE(panmar): GLFW_REPEAT is converted to GLFW_PRESS
        keys[key] = (action == GLFW_RELEASE) ? GLFW_RELEASE : GLFW_PRESS;
    }

    void on_mouse_button_changed(i32 key, i32 action, i32 mods) {
        mouse[key] = action == GLFW_RELEASE ? GLFW_RELEASE : GLFW_PRESS;
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

    bool is_mouse_button_down(i32 key) const {
        assert(key < static_cast<i32>(keys.size()));
        return mouse[key] == GLFW_PRESS;
    }

    bool is_mouse_button_pressed(i32 key) const {
        assert(key < static_cast<i32>(keys.size()));
        return mouse[key] == GLFW_PRESS && prev_mouse[key] == GLFW_RELEASE;
    }

    std::vector<f32> scroll_x_offsets;
    std::vector<f32> scroll_y_offsets;
    f32 cursor_pos_x = 0.f;
    f32 cursor_pos_y = 0.f;
    f32 prev_cursor_pos_x = 0.f;
    f32 prev_cursor_pos_y = 0.f;

private:
    array<i32, GLFW_KEY_LAST + 1> keys;
    array<i32, GLFW_KEY_LAST + 1> prev_keys;

    std::array<i32, GLFW_MOUSE_BUTTON_LAST> mouse;
    std::array<i32, GLFW_MOUSE_BUTTON_LAST> prev_mouse;
};
