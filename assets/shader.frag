#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

layout (location = 0) out vec4 fragColor;

void main() {
/*
    vec3 t = abs(vertColor - vec3(0.5)) * 2;
    float s = 1.0 - (length(t * t * t) - 1);
    s = float(s > 0.80);
    fragColor = s * vec4(vertColor, 0.0);
*/
    fragColor = vec4(dot(normalize(vec3(1, 1, -1)), norm));
}
