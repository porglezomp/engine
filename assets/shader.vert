#version 410

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 pos;
layout (location = 1) out vec3 vertColor;

uniform mat4 perspective;

void main() {
    gl_Position = vec4(vert - vec3(0.2, 0.2, 0.9), 1.0) * perspective;
    pos = vert;
    vertColor = color;
}
