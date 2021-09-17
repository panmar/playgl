#version 330

flat in vec3 start_position;
in vec3 vert_position;

out vec4 final_color;

uniform vec4 color;
uniform vec2 resolution;
uniform float dash_size;
uniform float gap_size;

void main() {
    vec2 dir = (vert_position.xy - start_position.xy) * resolution / 2.0;
    float dist = length(dir);

    if (fract(dist / (dash_size + gap_size)) >
        dash_size / (dash_size + gap_size))
        discard;
    final_color = color;
}