#define PGL_DEFINE_MAIN
#include "playgl.h"

bool pgl_init() { return true; }

void pgl_update(System& system){};

void pgl_render(System& system) {
    system.framebuffers("#main").color().depth().clear(Colors::White).bind();

    system.debug.gizmo().position(vec3(0.f, 1.f, 0.f));
    system.debug.grid().edge(10.f);
    system.debug.grid()
        .position(vec3(10.f, 0.f, 0.f))
        .edge(1.f)
        .color(vec4(1.f, 0.f, 0.f, 1.f));

    system.geometry.render(
        geometry::Quad{},
        system.content.shader("solid.vs", "solid.fs")
            .param("world", mat4(1.f))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("tex", system.content.texture("test.jpg")));

    system.geometry.render(
        geometry::WireCube{},
        system.content.shader("debug_dash.vs", "debug_dash.fs")
            .param("world", glm::scale(vec3(2.f)))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("color", vec4(0.f, 1.f, 1.f, 1.f)));

    system.geometry.render(
        geometry::WirePyramid{},
        system.content.shader("debug_dash.vs", "debug_dash.fs")
            .param("world", glm::scale(vec3(3.f)))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("color", vec4(1.f, 1.f, 0.f, 1.f)));

    system.geometry.render(
        geometry::WireSphere{10, 10},
        system.content.shader("debug_dash.vs", "debug_dash.fs")
            .param("world", mat4(1.f))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("color", vec4(0.f, 1.f, 0.f, 1.f)));

    system.postprocess("#main")
        .with("inverse.fs")
        .resulting("#inverse");

    system.framebuffers("#inverse").present();
};

// clang-format off
//
// USE CASE IDEAS:
//
// scene.model("boy")
//      .animation("run")               // <---- Optional
//      .loop()                         // <---- Optional
//      .play(elapsed_seconds)
//
// scene.collect("boy", "tree", "floor")
//      .update(system.timer.elapsed_time);
//
// clang-format on