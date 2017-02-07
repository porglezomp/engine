#ifndef GARDEN_LIB_RESOURCE_SHADER_HEADER_INCLUDED
#define GARDEN_LIB_RESOURCE_SHADER_HEADER_INCLUDED

#include "lib/gl.h"
#include <stddef.h>


#define UNIFORM_MAX_COUNT 8

struct Shader_Resource {
    const char *vert_fname;
    const char *frag_fname;
    size_t uniform_count;
    GLuint program;
    GLint uniforms[UNIFORM_MAX_COUNT];
};


void shader_bind_uniforms(Shader_Resource *shader,
                          size_t uniform_count, const char *names[]);


#endif
