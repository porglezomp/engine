#version 410
layout (location = 0) in vec3 vert;
layout (location = 0) out vec3 pos;

void main() {
    gl_Position = vec4(vert, 1.0);
    pos = vert;
}
