#define PGL_DEFINE_MAIN
#include "playgl.h"

#include "content.h"
// TODO(panmar): basic renderering functionality should stay in src/graphics and
// be included through 'playgl.h'; custom and scene specific rendering
// functinality should be included by a particular scene
#include "graphics/opengl/renderer.h"


Camera camera{CameraCanvas{"CAMERA"}, PerspectiveCamera{"CAMERA"}};
OrbitCameraController camera_controller{"main_camera_controller"};

bool pgl_init() {
    RendererGL4::set_window_hints();
    return true;
}

void pgl_update(const Timer& timer, const Input& input) {
    camera_controller.update(camera, input);

    // auto& boy = MODEL("boy");
    // if (!boy.animation_controller) {
    //     boy.animation_controller.reset(new AnimationController(boy.data));
    //     boy.animation_controller->play("Armature|ArmatureAction");
    // }

    // boy.update(elapsed_seconds);
};

void pgl_render(const Timer& timer) {
    RendererGL4::begin_frame(camera, true);

    SHADER("solid.vs", "solid.fs");

    // RendererGL2::render_gizmo(glm::vec3(0.f, 0.f, 0.f));
    // RendererGL2::render_grid(glm::vec3(0.f, 0.f, 0.f), 10.f,
    //                          Color(1.f, 1.f, 1.f, 1.f));

    // RendererGL2::render_model(MODEL("boy"));
};
