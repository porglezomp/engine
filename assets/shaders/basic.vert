#version 410

out vec3 frag_norm;

uniform mat4 world_from_object;
uniform mat4 view_from_world;
uniform mat4 proj_from_view;

in vec3 pos;
in vec3 norm;

void main() {
    gl_Position = proj_from_view * view_from_world * world_from_object * vec4(pos, 1);
    frag_norm = norm;
}
