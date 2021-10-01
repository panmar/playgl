#version 330 core

out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D tex0;
uniform float gamma;

void main() {
    FragColor = texture(tex0, tex_coords);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.f / gamma));
}