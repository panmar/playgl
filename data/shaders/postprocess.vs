#version 330 core

in vec3 IN_POSITION;
in vec2 IN_TEXCOORD;

out vec2 tex_coords;

void main() {
    gl_Position = vec4(IN_POSITION, 1.0);
    tex_coords = IN_TEXCOORD;
}