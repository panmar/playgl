#pragma once

#include "common.h"
#include "store.h"
#include "graphics/renderer.h"

class Scene {
public:
    void update(Store& store) {
        f32 elapsed_seconds = store.get<f32>("TIME_ELAPSED_SECONDS");
    }

    void render(const Store& store) const {
        RendererGL2::setup(store);
        RendererGL2::render_gizmo(glm::vec3(0.f, 0.f, 0.f));
    }
};