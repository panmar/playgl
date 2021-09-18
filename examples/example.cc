#define PGL_DEFINE_MAIN
#include "playgl.h"

bool pgl_init() { return true; }

void pgl_update(SystemRef& system){};

void pgl_render(SystemRef& system) {
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
// renderer.model("boy").world(???)
//         .model("tree").world(???)
//         .primitive(Primitive::Sphere).world(???)
//         .view(light.view)
//         .projection(light.projection)
//         .shader("defered_color")
//         .render();
//
// renderer.primitive(Primitive::Sphere)
//         .world(mat4(1.f))              // <---- Optional[World]
//         .view(camera.view)             // <---- Optional[View]
//         .projection(camera.projection) // <---- Optional[Projection]
//         .shader("solid")               // <---- Optional[Shader]
//         .param("WORLD", mat4(1.f))     // <---- Optional
//         .param("MY_PARAM", 42)         // <---- Optional
//         .nodepth()                     // <---- Optional
//         .nobuiltins()                  // <---- Optional
//         .render()
//
//
// renderer.model("boy")
//         .world(mat4(1.f))              // <---- World
//         .view(camera.view)             // <---- Optional[View]
//         .projection(camera.projection) // <---- Optional[Projection]
//         .shader("model")               // <---- Optional[Shader]
//         .param("MY_PARAM", 42)         // <---- Optional
//         .render()
//
//
// postprocess.input("in1")                    // <---- Optional, if none assume MAIN
//            .input("in2")                    // <---- Optional, if none assume MAIN
//            .input(FrameBuffer::Color)       // <---- Optional, MAIN framebuffer
//            .input(FrameBuffer::Depth)       // <---- Optional, MAIN framebuffer
//            .output("bloom")                 // <---- Optional, assume same size as MAIN
//            .output("sepia", 800, 600)       // <---- Optional, if none assume MAIN
//            .shader("bloom")                 // <---- Optional[Shader]
//            .param("MY_PARAM_NAME", 42)      // <---- Optional
//            .debug()                         // <---- Optional, if true render thumbnail .render()
//
// clang-format on