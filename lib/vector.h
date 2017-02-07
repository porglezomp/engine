#ifndef GARDEN_VECTOR_HEADER_INCLUDED
#define GARDEN_VECTOR_HEADER_INCLUDED

#include "gl.h"


typedef struct Vec4 {
    GLfloat x, y, z, w;
} Vec4;

typedef struct Vec3 {
    GLfloat x, y, z;
} Vec3;


GLfloat vec4_dot(const Vec4 *a, const Vec4 *b);
GLfloat vec3_dot(const Vec3 *a, const Vec3 *b);

void vec4_addi(Vec4 *a, const Vec4 *b);
Vec4 vec4_add(const Vec4 *a, const Vec4 *b);

void vec3_addi(Vec3 *a, const Vec3 *b);
Vec3 vec3_add(const Vec3 *a, const Vec3 *b);


#endif
