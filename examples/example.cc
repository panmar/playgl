#define PGL_DEFINE_MAIN
#include "playgl.h"

void pgl_init(Store& store) {
    store["screen_color"] = Color(0.5f, 0.5f, 0.5);
    store["PHONG_COLOR"] = Color(0.7f, 0.4f, 0.3f);
    store["LIGHT_COLOR"] = Color(0.8f, 0.2f, 0.4f);

    store["test_bounded"] = BoundedParam(vec4(-1.f, 15.f, 32.f, -3.f), -5, 50);
}

void pgl_update(System& system) {
    system.camera.canvas.color = system.store["screen_color"];
};

void pgl_render(System& system) {
    system.geometry.render(
        geometry::TrefoilKnot<>{},
        system.content.shader("phong.vs", "phong.fs")
            .param("world", mat4(1.f))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection()));

    system.debug.grid().edge(10.f);
    system.debug.model("test.glb");
    system.debug.texture(
        system.camera.canvas.framebuffer->color_texture.value());

    // system.postprocess(*system.camera.canvas.framebuffer)
    //     .with("grayscale.fs")
    //     .resulting("#grayscale");

    // system.postprocess("#grayscale")
    //     .with("postprocess.fs")
    //     .resulting(*system.camera.canvas.framebuffer);
};
