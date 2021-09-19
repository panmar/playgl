#version 330 core

out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D tex;

void main() { FragColor = vec4(vec3(1.0 - texture(tex, tex_coords)), 1.0); }