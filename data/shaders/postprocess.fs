#version 330 core

out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D tex;

void main() { FragColor = texture(tex, tex_coords); }