#ifndef GARDEN_LIB_RESOURCE_SHADER_HEADER_INCLUDED
#define GARDEN_LIB_RESOURCE_SHADER_HEADER_INCLUDED

#include "lib/gl.h"


typedef struct Shader_Resource {
    const char *vert_fname;
    const char *frag_fname;
    GLuint program;
} Shader_Resource;


#endif
