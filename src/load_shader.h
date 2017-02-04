#ifndef GARDEN_LOAD_SHADER_HEADER_INCLUDED
#define GARDEN_LOAD_SHADER_HEADER_INCLUDED

#include "../lib/gl.h"


typedef enum Shader_Load_Error {
    Shader_Load_Error_None,
    Shader_Load_Error_Opening_File,
    Shader_Load_Error_Compiling_Shader,
} Shader_Load_Error;

typedef struct Resource_Error {
    char *message;
} Resource_Error;

typedef struct Shader_Resource {
    char *vert_fname;
    char *frag_fname;
    GLuint program;
} Shader_Resource;

Shader_Load_Error shader_load(Shader_Resource *resource, Resource_Error *err);
void free_resource_error(Resource_Error *err);


#endif
