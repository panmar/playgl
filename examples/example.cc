#define PGL_DEFINE_MAIN
#include "playgl.h"

void pgl_init(Store& store) {
    store["PHONG_COLOR"].is(StoreParam::Gui | StoreParam::Shader) =
        Color(0.7f, 0.4f, 0.3f);
    store["MY_PARAM2"].is(StoreParam::Gui) = 12345;
}

void pgl_update(System& system){};

void pgl_render(System& system) {
    system.framebuffers("#main").color().depth().clear().bind();

    system.debug.gizmo().position(vec3(0.f, 1.f, 0.f));
    system.debug.grid().edge(10.f);
    system.debug.grid()
        .position(vec3(10.f, 0.f, 0.f))
        .edge(1.f)
        .color(Color(1.f, 0.f, 0.f, 1.f));

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
            .param("color", Color(0.f, 1.f, 1.f, 1.f)));

    system.geometry.render(
        geometry::WirePyramid{},
        system.content.shader("debug_dash.vs", "debug_dash.fs")
            .param("world", glm::scale(vec3(3.f)))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("color", Color(1.f, 1.f, 0.f, 1.f)));

    system.geometry.render(
        geometry::WireSphere{10, 10},
        system.content.shader("debug_dash.vs", "debug_dash.fs")
            .param("world", mat4(1.f))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("color", Color(0.f, 1.f, 0.f, 1.f)));

    system.geometry.render(
        geometry::TrefoilKnot{},
        system.content.shader("phong.vs", "phong.fs")
            .param("world", glm::translate(vec3(3.f, 0.f, 3.f)))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("light_pos", vec3(5.f, 5.f, 5.f))
            .param("view_pos", system.camera.geometry.get_position())
            .param("light_color", Color(0.8f, 0.2f, 0.4f)));

    system.geometry.render(
        geometry::extract_normals(geometry::Dodecahedron{}),
        system.content.shader("debug_dash.vs", "debug_dash.fs")
            .param("world", glm::translate(vec3(3.f, 0.f, 3.f)))
            .param("view", system.camera.geometry.get_view())
            .param("projection", system.camera.geometry.get_projection())
            .param("color", Color(0.f, 1.f, 0.f, 1.f)));

    system.postprocess("#main").with("grayscale.fs").resulting("#grayscale");

    system.framebuffers("#grayscale").present();
};
