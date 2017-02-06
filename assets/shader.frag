#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

layout (location = 0) out vec4 fragColor;

void main() {
    vec3 normal = normalize(norm);
    float leftLight = dot(normal, normalize(vec3(-2, 0, -1)));
    float rightLight = dot(normal, normalize(vec3(2, 1, 0)));
    vec3 color = vec3(1, 0.9, 0.9) * leftLight + vec3(0.9, 0.9, 1) * rightLight;
    fragColor = vec4(color, 1.0);
}
