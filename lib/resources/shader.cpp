#include "shader.h"

void
shader_bind_uniforms(Shader_Resource *shader,
                     size_t uniform_count, const char *names[])
{
    // @Robustness: Handle missing uniform values
    shader->uniform_count = uniform_count;
    for (size_t i = 0; i < uniform_count; ++i) {
        shader->uniforms[i] = glGetUniformLocation(shader->program, names[i]);
    }
}
