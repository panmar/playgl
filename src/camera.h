#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "common.h"
#include "input.h"

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

private:
    glm::vec3 position = glm::vec3{1.f, 1.f, 1.f};
    glm::vec3 target = glm::normalize(glm::vec3{-1.f, -1.f, -1.f});
    glm::vec3 up = glm::normalize(glm::cross(target, glm::normalize(glm::vec3{1.f, 2.f, 3.f})));

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
    void update(PerspectiveCamera* camera, const Input& input) {}
};
