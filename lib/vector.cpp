#include "vector.h"


GLfloat
vec4_dot(const Vec4 *a, const Vec4 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

GLfloat
vec3_dot(const Vec3 *a, const Vec3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void
vec4_addi(Vec4 *a, const Vec4 *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
    a->w += b->w;
}

Vec4
vec4_add(const Vec4 *a, const Vec4 *b)
{
    Vec4 result = *a;
    vec4_addi(&result, b);
    return result;
}

void
vec3_addi(Vec3 *a, const Vec3 *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

Vec3
vec3_add(const Vec3 *a, const Vec3 *b)
{
    Vec3 result = *a;
    vec3_addi(&result, b);
    return result;
}
