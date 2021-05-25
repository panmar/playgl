#pragma once

#include "common.h"
#include "content.h"
#include "store.h"
#include "graphics/renderer.h"

namespace Scene {

void pre_graphics_startup() { RendererGL2::set_window_hints(); }

void update(Store& store) {
    f32 elapsed_seconds = store.get<f32>(StoreParams::kTimeElapsedSeconds);

    auto& boy = MODEL("boy");
    if (!boy.animation_controller) {
        boy.animation_controller.reset(
            new AnimationController(boy.data));
        boy.animation_controller->play("Armature|ArmatureAction");
    }

    boy.update(elapsed_seconds);
}

void render(const Store& store) {
    RendererGL2::begin_frame(store);
    RendererGL2::render_gizmo(glm::vec3(0.f, 0.f, 0.f));
    RendererGL2::render_grid(glm::vec3(0.f, 0.f, 0.f), 10.f,
                             glm::vec3(1.f, 1.f, 1.f));

    RendererGL2::render_model(MODEL("boy"), 100.f);
}

}  // namespace Scene
