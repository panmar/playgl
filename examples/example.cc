#define PGL_DEFINE_MAIN
#include "playgl.h"

void pgl_init(Store& store) {
    store["screen_color"] = Color(0.5f, 0.5f, 0.5f);
    store["PHONG_COLOR"] = Color(0.7f, 0.4f, 0.3f);
    store["LIGHT_COLOR"] = Color(0.8f, 0.2f, 0.4f);
}

void pgl_update(System& system) {
    system.camera.canvas.color = system.store["screen_color"];
    system.camera.canvas.framebuffer =
        &system.framebuffers("#main").color().depth();
};

void pgl_render(System& system) {
    system.camera.canvas.clear();

    system.debug.grid().edge(10.f);

    system.debug.model("test.glb");

    system.postprocess("#main").with("grayscale.fs").resulting("#grayscale");

    system.framebuffers("#grayscale").present();
};
