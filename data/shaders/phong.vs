#version 330 core

in vec3 IN_POSITION;
in vec3 IN_NORMAL;

out vec3 OUT_NORAML;
out vec3 OUT_FRAGMENT_POSITION;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = vec4(IN_POSITION, 1.0) * world * view * projection;
    OUT_NORAML = IN_NORMAL * mat3(inverse(transpose(world)));
    OUT_FRAGMENT_POSITION = vec3(vec4(IN_POSITION, 1.0) * world);
}