#ifndef GARDEN_LIB_RESOURCE_SHADER_HEADER_INCLUDED
#define GARDEN_LIB_RESOURCE_SHADER_HEADER_INCLUDED

#include "lib/gl.h"


typedef struct Shader_Resource {
    char *vert_fname;
    char *frag_fname;
    GLuint program;
} Shader_Resource;


#endif
