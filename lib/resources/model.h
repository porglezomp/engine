#ifndef GARDEN_MODEL_HEADER_INCLUDED
#define GARDEN_MODEL_HEADER_INCLUDED

#include "lib/gl.h"
#include <stddef.h>


#define ATTRIB_MAX_COUNT 8

typedef struct Shader_Resource Shader_Resource;

typedef struct Vertex_Format {
    size_t attribs_count;
    GLsizei stride;
    intptr_t offsets[ATTRIB_MAX_COUNT];
    GLint sizes[ATTRIB_MAX_COUNT];
    GLenum types[ATTRIB_MAX_COUNT];
} Vertex_Format;

typedef struct Model_Resource {
    const char *model_fname;
    size_t index_count;
    Shader_Resource *shader;
    Vertex_Format format;
    GLuint vao, vbo, ibo;
} Model_Resource;

typedef struct Vertex_XYZ_RGB {
    GLfloat x, y, z;
    GLfloat r, g, b;
} Vertex_XYZ_RGB;

extern Vertex_Format vertex_format_xyz_rgb;

void bind_model(Model_Resource *model);
void build_model(Model_Resource *model,
                 void *data, size_t data_count,
                 GLuint *indices, size_t index_count);


#endif
