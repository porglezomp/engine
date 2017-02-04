#version 410

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 pos;
layout (location = 1) out vec3 vertColor;

void main() {
    gl_Position = vec4(vert, 1.0);
    pos = vert;
    vertColor = color;
}
