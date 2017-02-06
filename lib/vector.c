#include "vector.h"


GLfloat
vec4_dot(const Vec4 *a, const Vec4 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
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
