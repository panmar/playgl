#version 330 core

in vec3 IN_POSITION;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = vec4(IN_POSITION.x, IN_POSITION.y, IN_POSITION.z, 1.0) *
                  world * view * projection;
}
