#ifndef GARDEN_SHADER_HEADER_INCLUDED
#define GARDEN_SHADER_HEADER_INCLUDED

#include "../lib/gl.h"


typedef enum Shader_Error_Type {
    Shader_Error_Unknown,
    Shader_Error_Vert,
    Shader_Error_Frag,
    Shader_Error_Link,
} Shader_Error_Type;

typedef struct Shader_Error {
    char *message;
    Shader_Error_Type type;
} Shader_Error;

GLuint compile_shader(const char *vert_src, const char *frag_src,
                      Shader_Error *err);
void free_shader_error(Shader_Error *err);


#endif
