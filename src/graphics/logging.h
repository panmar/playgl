#pragma once

#include "common.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

namespace debug {

inline void APIENTRY opengl_debug_callback(GLenum source, GLenum type, u32 id,
                                           GLenum severity, GLsizei length,
                                           const char* message,
                                           const void* user_param) {
    u32 ids_to_ignore[] = {131169, 131185, 131218, 131204};
    for (auto id_to_ignore : ids_to_ignore) {
        if (id == id_to_ignore) {
            return;
        }
    }

    u32 types_to_ignore[] = {GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_TYPE_POP_GROUP};
    for (auto& type_to_ignore : types_to_ignore) {
        if (type == type_to_ignore) {
            return;
        }
    }

    printf("---------------\n");
    printf(fmt::format("Debug message ({}): {}\n", id, message).c_str());

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            printf("Source: API");
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            printf("Source: Window System");
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            printf("Source: Shader Compiler");
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            printf("Source: Third Party");
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            printf("Source: Application");
            break;
        case GL_DEBUG_SOURCE_OTHER:
            printf("Source: Other");
            break;
    }
    printf("\n");

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            printf("Type: Error");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            printf("Type: Deprecated Behaviour");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            printf("Type: Undefined Behaviour");
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            printf("Type: Portability");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            printf("Type: Performance");
            break;
        case GL_DEBUG_TYPE_MARKER:
            printf("Type: Marker");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            printf("Type: Push Group");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            printf("Type: Pop Group");
            break;
        case GL_DEBUG_TYPE_OTHER:
            printf("Type: Other");
            break;
    }
    printf("\n");

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            printf("Severity: high");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            printf("Severity: medium");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            printf("Severity: low");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            printf("Severity: notification");
            break;
    }
    printf("\n\n");
}

inline void setup_logging() {
    i32 flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_debug_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                              nullptr, GL_TRUE);
    }
}

inline void scope_start(const char* name) {
    if (glPushDebugGroup) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name);
    }
}

inline void scope_end() {
    if (glPopDebugGroup) {
        glPopDebugGroup();
    }
}

struct Scope {
    Scope(const char* name) { scope_start(name); }
    ~Scope() { scope_end(); }
};

#define DEBUG_SCOPE(name) \
    debug::Scope scope { STRINGIFY(name) }

inline void label(const string& name, u32 type, u32 resource) {
    if (glObjectLabel && !name.empty()) {
        glObjectLabel(type, resource, -1, name.c_str());
    }
}

}  // namespace debug
