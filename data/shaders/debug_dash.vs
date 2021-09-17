#version 330

in vec3 IN_POSITION;

flat out vec3 start_position;
out vec3 vert_position;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 pos = vec4(IN_POSITION, 1.0) * world * view * projection;
    gl_Position = pos;
    vert_position = pos.xyz / pos.w;
    start_position = vert_position;
}