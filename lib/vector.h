#ifndef GARDEN_VECTOR_HEADER_INCLUDED
#define GARDEN_VECTOR_HEADER_INCLUDED

#include "gl.h"


typedef struct Vec4 {
    GLfloat x, y, z, w;
} Vec4;


GLfloat vec4_dot(const Vec4 *a, const Vec4 *b);

void vec4_addi(Vec4 *a, const Vec4 *b);
Vec4 vec4_add(const Vec4 *a, const Vec4 *b);


#endif
