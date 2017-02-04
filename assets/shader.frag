#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertColor;

layout (location = 0) out vec4 fragColor;

void main() {
    vec3 t = abs(vertColor - vec3(0.5)) * 2;
    fragColor = (0.7 - (length(t * t * t) - 1)) * vec4(vertColor, 0.0);
}
