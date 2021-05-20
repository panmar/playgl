#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>

#include "common.h"
#include "input.h"
#include "settings.h"

class Camera {
public:
    Camera() {}
    Camera(const vec3& position, const vec3& target, const vec3& up)
        : position(position), target(target), up(up) {}
    virtual ~Camera() = default;

    vec3 get_position() const { return position; }
    vec3 get_target() const { return target; }
    vec3 get_up() const { return up; }
    vec3 get_down() const { return -get_up(); }
    vec3 get_forward() const { return glm::normalize(target - position); }
    vec3 get_backward() const { return -get_forward(); }
    vec3 get_left() const {
        return glm::normalize(glm::cross(get_up(), get_forward()));
    }
    vec3 get_right() const { return -get_left(); }

    const mat4& get_view() const {
        if (!is_view_valid) {
            view = glm::lookAt(position, target, up);
            is_view_valid = true;
        }
        return view;
    }

    void set_position(const vec3& value) {
        position = value;
        is_view_valid = false;
    }

    void set_target(const vec3& value) {
        target = value;
        is_view_valid = false;
    }

    void set_up(const vec3& value) {
        up = value;
        is_view_valid = false;
    }

    virtual const mat4& get_projection() const = 0;

    void rotate_around_target(f32 left_rot, f32 up_rot) {
        glm::mat4 rot(1.f);
        rot = glm::rotateNormalizedAxis(rot, -left_rot, get_left());
        rot = glm::rotateNormalizedAxis(rot, -up_rot, glm::vec3(0.f, 1.f, 0.f));
        auto direction = target - position;
        glm::vec3 new_direction = glm::vec4(direction, 0.f) * rot;
        glm::vec3 new_up = glm::vec4(up, 0.f) * rot;

        auto new_value =
            glm::dot(glm::normalize(new_direction), glm::vec3(0.f, 1.f, 0.f));

        if (new_value < -0.999f || new_value > 0.f) {
            return;
        }

        set_position(target - new_direction);
        set_up(new_up);
    }

private:
    glm::vec3 position = glm::vec3{10.f, 10.f, 10.f};
    glm::vec3 target = glm::normalize(glm::vec3{-1.f, -1.f, -1.f});
    glm::vec3 up = glm::normalize(
        glm::cross(target, glm::normalize(glm::vec3{1.f, 2.f, 3.f})));

    mutable mat4 view;
    mutable bool is_view_valid = false;
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera() {}
    PerspectiveCamera(f32 aspect_ratio, f32 fov, f32 near, f32 far)
        : aspect_ratio(aspect_ratio), fov(fov), near(near), far(far) {}
    virtual ~PerspectiveCamera() = default;

    f32 get_aspect_ratio() const { return aspect_ratio; }
    f32 get_fov() const { return fov; }
    f32 get_near() const { return near; }
    f32 get_far() const { return far; }

    const mat4& get_projection() const override {
        if (!is_valid_projection) {
            projection = glm::perspective(fov, aspect_ratio, near, far);
            is_valid_projection = true;
        }
        return projection;
    }

private:
    f32 aspect_ratio = 1.f;
    f32 fov = 1.1623f;
    f32 near = 0.1f;
    f32 far = 100.f;

    mutable mat4 projection;
    mutable bool is_valid_projection = false;
};

class OrbitCameraController {
public:
    void update(PerspectiveCamera& camera, const Input& input) {
        auto left_rot = 0.f, up_rot = 0.f;
        auto left_rot_step = 0.1f, up_rot_step = 0.1f;
        if (input.is_key_down(Settings::key_rotate_left)) {
            left_rot += left_rot_step;
        }
        if (input.is_key_down(Settings::key_rotate_right)) {
            left_rot -= left_rot_step;
        }
        if (input.is_key_down(Settings::key_rotate_up)) {
            up_rot -= up_rot_step;
        }
        if (input.is_key_down(Settings::key_rotate_down)) {
            up_rot += up_rot_step;
        }

        if (input.is_mouse_button_down(Settings::key_camera_rotate)) {
            auto mouse_left_rot_step = 0.005f, mouse_up_rot_step = 0.005f;
            left_rot = -(input.cursor_pos_x - input.prev_cursor_pos_x) *
                       mouse_left_rot_step;
            up_rot = (input.cursor_pos_y - input.prev_cursor_pos_y) *
                     mouse_up_rot_step;
        }

        camera.rotate_around_target(up_rot, left_rot);

        if (input.is_key_down(Settings::key_zoom_in)) {
            auto position = camera.get_position();
            auto forward = camera.get_forward();
            auto step = 1.0f;
            auto new_position = position + forward * step;
            auto dist = glm::distance(glm::vec3(0.f), new_position);
            if (dist > min_zoom && dist < max_zoom) {
                camera.set_position(new_position);
            }
        }

        if (input.is_key_down(Settings::key_zoom_out)) {
            auto position = camera.get_position();
            auto backward = camera.get_backward();
            auto step = 1.0f;
            auto new_position = position + backward * step;
            auto dist = glm::distance(glm::vec3(0.f), new_position);
            if (dist > min_zoom && dist < max_zoom) {
                camera.set_position(new_position);
            }
        }

        for (auto& step : input.scroll_x_offsets) {
            auto position = camera.get_position();
            auto forward = camera.get_position();
            auto new_position = position + forward * -step;
            auto dist = glm::distance(glm::vec3(0.f), new_position);
            if (dist > min_zoom && dist < max_zoom) {
                camera.set_position(new_position);
            }
        }

        for (auto& step : input.scroll_y_offsets) {
            auto position = camera.get_position();
            auto backward = camera.get_backward();
            auto new_position = position + backward * -step;
            auto dist = glm::distance(glm::vec3(0.f), new_position);
            if (dist > min_zoom && dist < max_zoom) {
                camera.set_position(new_position);
            }
        }
    }

    const f32 min_zoom = 0.1f;
    const f32 max_zoom = 50.f;
};
