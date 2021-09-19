#version 330 core

out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, tex_coords);
    float average =
        0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}