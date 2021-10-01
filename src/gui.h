#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "store.h"

namespace Gui {

inline bool startup(GLFWwindow* window) {
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

inline void update() {}

inline void render_param_with_slider(const string& name, StoreParam& param) {
    std::visit(
        [&name, &param](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, i32>) {
                ImGui::SliderInt(name.c_str(), &arg, param.min_bound,
                                 param.max_bound);
            } else if constexpr (std::is_same_v<T, f32>) {
                ImGui::SliderFloat(name.c_str(), &arg, param.min_bound,
                                   param.max_bound);
            } else if constexpr (std::is_same_v<T, vec2>) {
                ImGui::SliderFloat2(name.c_str(), &arg.x, param.min_bound,
                                    param.max_bound);
            } else if constexpr (std::is_same_v<T, vec3>) {
                ImGui::SliderFloat3(name.c_str(), &arg.x, param.min_bound,
                                    param.max_bound);
            } else if constexpr (std::is_same_v<T, vec4>) {
                ImGui::SliderFloat4(name.c_str(), &arg.x, param.min_bound,
                                    param.max_bound);
            } else if constexpr (std::is_same_v<T, Color>) {
                // NOTE(panmar): A color picker needs to work on gamma corrected
                // values. The store is storing colors in linear space.
                Color color = Colors::gamma_corrected(arg, config::gamma);
                ImGui::ColorEdit4(name.c_str(), color.data);
                arg = Colors::to_linear(color, config::gamma);
            } else if constexpr (std::is_same_v<T, mat4>) {
                // NOTE(panmar): Not supported
            } else if constexpr (std::is_same_v<T, string>) {
                // NOTE(panmar): Not supported
            } else {
                static_assert(false);
            }
        },
        param.param);
}

inline void render_param(const string& name, StoreParam& param) {
    std::visit(
        [&name](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, i32>) {
                ImGui::InputInt(name.c_str(), &arg);
            } else if constexpr (std::is_same_v<T, f32>) {
                ImGui::InputFloat(name.c_str(), &arg);
            } else if constexpr (std::is_same_v<T, vec2>) {
                ImGui::InputFloat2(name.c_str(), &arg.x);
            } else if constexpr (std::is_same_v<T, vec3>) {
                ImGui::InputFloat3(name.c_str(), &arg.x);
            } else if constexpr (std::is_same_v<T, vec4>) {
                ImGui::InputFloat4(name.c_str(), &arg.x);
            } else if constexpr (std::is_same_v<T, Color>) {
                // NOTE(panmar): A color picker needs to work on gamma corrected
                // values. The store is storing colors in linear space.
                Color color = Colors::gamma_corrected(arg, config::gamma);
                ImGui::ColorEdit4(name.c_str(), color.data);
                arg = Colors::to_linear(color, config::gamma);
            } else if constexpr (std::is_same_v<T, mat4>) {
                // NOTE(panmar): Not supported
            } else if constexpr (std::is_same_v<T, string>) {
                // NOTE(panmar): Not supported
            } else {
                static_assert(false);
            }
        },
        param.param);
}

inline void render(Store& store) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("User params");

        for (auto& key_value : store) {
            auto& name = key_value.first;
            auto& param = key_value.second;
            if (param.has(StoreParam::Gui)) {
                if (param.has(StoreParam::Bounded)) {
                    render_param_with_slider(name, param);
                } else {
                    render_param(name, param);
                }
            }
        }

        ImGui::End();
    }

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

inline void shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

};  // namespace Gui
