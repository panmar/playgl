#pragma once

#include "common.h"
#include "config.h"
#include "input.h"
#include "config.h"
#include "graphics/opengl/framebuffer.h"

class CameraGeometry {
public:
    CameraGeometry() = default;
    CameraGeometry(const vec3& position, const vec3& target, const vec3& up) {
        this->position = position;
        this->target = target;
        this->up = up;
    }
    virtual ~CameraGeometry() = default;

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
    vec3 position = glm::vec3{10.f, 10.f, 10.f};
    vec3 target = glm::vec3{0.f, 0.f, 0.f};
    vec3 up =
        glm::normalize(glm::cross(glm::normalize(target - position),
                                  glm::normalize(glm::vec3{1.f, 2.f, 3.f})));

    mutable bool is_view_valid = false;
    mutable mat4 view;
};

class PerspectiveCamera : public CameraGeometry {
public:
    PerspectiveCamera() = default;
    PerspectiveCamera(f32 aspect_ratio, f32 fov, f32 _near, f32 _far)
        : aspect_ratio(aspect_ratio), fov(fov), _near(_near), _far(_far) {}
    virtual ~PerspectiveCamera() = default;

    f32 get_aspect_ratio() const { return aspect_ratio; }
    f32 get_fov() const { return fov; }
    f32 get_near() const { return _near; }
    f32 get_far() const { return _far; }

    void set_aspect_ratio(f32 value) {
        aspect_ratio = value;
        is_valid_projection = false;
    }

    void set_fov(f32 value) {
        fov = value;
        is_valid_projection = false;
    }

    void set_near(f32 value) {
        _near = value;
        is_valid_projection = false;
    }

    void set_far(f32 value) {
        _far = value;
        is_valid_projection = false;
    }

    const mat4& get_projection() const override {
        if (!is_valid_projection) {
            if (config::inverse_depth) {
            projection =
                compute_inverse_depth_inf_perspective(fov, aspect_ratio, _near);
            } else {
                projection = glm::perspective(fov, aspect_ratio, _near, _far);
            }

            is_valid_projection = true;
        }
        return projection;
    }

private:
    f32 aspect_ratio = 1.f;
    f32 fov = 1.1623f;
    f32 _near = 0.1f;
    f32 _far = 100.f;

    mutable mat4 projection;
    mutable bool is_valid_projection = false;
};

class CameraCanvas {
public:
    void clear() {
        if (!framebuffer) {
            return;
        }

        framebuffer->clear(color);
    }

    i32 width = 800;
    i32 height = 600;
    Color color = Color(0.5f, 0.5f, 0.5f, 1.f);
    Framebuffer* framebuffer = nullptr;
};

struct Camera {
    CameraCanvas canvas;
    PerspectiveCamera geometry;
};

class OrbitCameraController {
public:
    void update(Camera& camera, const Input& input) {
        CameraCanvas& camera_canvas = camera.canvas;
        PerspectiveCamera& camera_geometry = camera.geometry;

        auto left_rot = 0.f, up_rot = 0.f;
        auto left_rot_step = 0.1f, up_rot_step = 0.1f;

        auto camera_rotate_key = config::key_camera_rotate;
        if (input.is_mouse_button_down(camera_rotate_key)) {
            auto mouse_left_rot_step = 0.005f, mouse_up_rot_step = 0.005f;
            left_rot = -(input.cursor_pos_x - input.prev_cursor_pos_x) *
                       mouse_left_rot_step;
            up_rot = (input.cursor_pos_y - input.prev_cursor_pos_y) *
                     mouse_up_rot_step;
        }

        camera_geometry.rotate_around_target(up_rot, left_rot);

        for (auto& step : input.scroll_x_offsets) {
            auto position = camera_geometry.get_position();
            auto forward = camera_geometry.get_position();
            auto new_position = position + forward * -step;
            auto dist = glm::distance(glm::vec3(0.f), new_position);
            if (dist > min_zoom && dist < max_zoom) {
                camera_geometry.set_position(new_position);
            }
        }

        for (auto& step : input.scroll_y_offsets) {
            auto position = camera_geometry.get_position();
            auto backward = camera_geometry.get_backward();
            auto new_position = position + backward * -step;
            auto dist = glm::distance(glm::vec3(0.f), new_position);
            if (dist > min_zoom && dist < max_zoom) {
                camera_geometry.set_position(new_position);
            }
        }
    }

    f32 min_zoom = 0.1f;
    f32 max_zoom = 50.f;
};
