#pragma once

#include "common.h"
#include "store.h"
#include "graphics/renderer.h"

namespace Scene {

void update(Store& store) {
    f32 elapsed_seconds = store.get<f32>(StoreParams::kTimeElapsedSeconds);
}

void render(const Store& store) {
    RendererGL2::begin_frame(store);
    RendererGL2::render_gizmo(glm::vec3(0.f, 0.f, 0.f));
}

}  // namespace Scene
