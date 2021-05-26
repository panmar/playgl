#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>

#include "store.h"

namespace Gui {

bool startup(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        return false;
    }

    if (!ImGui_ImplOpenGL2_Init()) {
        return false;
    }

    return true;
}

void update() {}

void render() {
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Camera");

        auto camera_position =
            pgl_store().get<vec3>(StoreParams::kCameraPosition);
        auto camera_target = pgl_store().get<vec3>(StoreParams::kCameraTarget);
        auto camera_up = pgl_store().get<vec3>(StoreParams::kCameraUp);
        ImGui::InputFloat3("Position", &camera_position.x, 2);
        ImGui::InputFloat3("Target", &camera_target.x, 2);
        ImGui::InputFloat3("Up", &camera_up.x, 2);

        ImGui::End();
    }

    ImGui::Render();

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void shutdown() {
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

};  // namespace Gui
