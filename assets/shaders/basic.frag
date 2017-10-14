#version 410

out vec4 out_color;
in vec3 frag_norm;

// uniform vec4 color;
void main() {
    float x = dot(frag_norm, vec3(1.0, 0.0, 0.0));
    out_color = vec4(x);
}
