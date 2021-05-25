#define PGL_DEFINE_MAIN

#include "playgl.h"

// TODO(panmar): basic renderering functionality should stay in src/graphics and
// be included through 'playgl.h'; custom and scene specific rendering
// functinality should be included by a particular scene
#include "graphics/renderer.h"

void pgl_init() {
    auto& store = pgl_store();

    i32 width = 1920;
    i32 height = 1080;

    {
        store.set(StoreParams::kWindowTitle, "PlayGL: example scene");
        store.set(StoreParams::kFrameBufferWidth, width);
        store.set(StoreParams::kFrameBufferHeight, height);
    }

    {
        glm::vec3 position = glm::vec3{10.f, 10.f, 10.f};
        glm::vec3 target = glm::vec3{0.f, 0.f, 0.f};
        glm::vec3 up = glm::normalize(
            glm::cross(glm::normalize(target - position),
                       glm::normalize(glm::vec3{1.f, 2.f, 3.f})));

        store.set(StoreParams::kCameraPosition, position);
        store.set(StoreParams::kCameraTarget, target);
        store.set(StoreParams::kCameraUp, up);
        store.set(StoreParams::kCameraFov, 1.1623f);
        store.set(StoreParams::kCameraAspectRatio,
                  static_cast<f32>(width) / height);
        store.set(StoreParams::kCameraNear, 0.1f);
        store.set(StoreParams::kCameraFar, 100.f);
    }

    RendererGL2::set_window_hints();
}

void pgl_update() {
    auto& store = pgl_store();
    f32 elapsed_seconds = store.get<f32>(StoreParams::kTimeElapsedSeconds);

    auto& boy = MODEL("boy");
    if (!boy.animation_controller) {
        boy.animation_controller.reset(new AnimationController(boy.data));
        boy.animation_controller->play("Armature|ArmatureAction");
    }

    boy.update(elapsed_seconds);
};

void pgl_render() {
    auto& store = pgl_store();
    RendererGL2::begin_frame(store, true);
    RendererGL2::render_gizmo(glm::vec3(0.f, 0.f, 0.f));
    RendererGL2::render_grid(glm::vec3(0.f, 0.f, 0.f), 10.f,
                             glm::vec3(1.f, 1.f, 1.f));

    RendererGL2::render_model(MODEL("boy"), 100.f);
};
