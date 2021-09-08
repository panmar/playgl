#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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

    if (!ImGui_ImplOpenGL3_Init()) {
        return false;
    }

    return true;
}

void update() {}

void render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Camera");

        // vec3 camera_position =
        //     STORE[StoreParams::kCameraPosition];
        // vec3 camera_target = STORE[StoreParams::kCameraTarget];
        // vec3 camera_up = STORE[StoreParams::kCameraUp];
        // ImGui::InputFloat3("Position", &camera_position.x);
        // ImGui::InputFloat3("Target", &camera_target.x);
        // ImGui::InputFloat3("Up", &camera_up.x);

        ImGui::End();
    }

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

};  // namespace Gui
