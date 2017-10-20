#version 410

out vec4 out_color;
in vec3 frag_norm;

// uniform vec4 color;
void main() {
    const vec3 SUN = vec3(1.0, 0.0, 0.0);
    float direct = clamp(dot(SUN, frag_norm), 0.0, 1.0);
    float indirect = clamp(dot(SUN, frag_norm) + 0.9, 0.0, 1.0);
    vec3 color
        = direct * vec3(1.0, 0.7, 0.3)
        + indirect * vec3(0.1, 0.2, 0.3);
    out_color = vec4(color, 1.0);
}
