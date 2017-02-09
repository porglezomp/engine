#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 fragColor;

void main() {
    vec3 normal = normalize(norm);
    float leftLight = max(0, dot(normal, normalize(vec3(-2, 0, -1))));
    float rightLight = max(0, dot(normal, normalize(vec3(2, 1, 0))));
    vec3 color = vec3(0.5, 0.3, 0.31) * leftLight + vec3(0.9, 0.9, 1) * rightLight;
    const float scale = 128;
    float u = int(uv.x * scale) % 2;
    float v = int(uv.y * scale) % 2;
    fragColor = vec4(float(u == v) * color, 1.0);
}
