#version 410
layout (location = 0) out vec4 color;
layout (location = 0) in vec3 pos;

void main() {
    float t = abs(pos.x) + abs(pos.y);
    t = (max(t, 0.8) - 0.8) * 5;
    vec3 color0 = vec3(0.102, 0.467, 0.651);
    vec3 color1 = vec3(0.402, 0.71, 0.91);
    color = vec4(mix(color0, color1, t), 0.0);
}
