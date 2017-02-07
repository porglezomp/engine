#include "quaternion.h"

#include "vector.h"

#include <math.h>


#define RAD2DEG (180.0/M_PI)
#define DEG2RAD	(M_PI/180.0)

Quat
quat_angle_axis(GLfloat degrees, Vec3 *axis)
{
    GLfloat radians = degrees * DEG2RAD/2;
    GLfloat si = sin(radians);
    GLfloat co = cos(radians);
    GLfloat l = sqrt(vec3_dot(axis, axis));
    Quat result;
    result.x = si * axis->x / l;
    result.y = si * axis->y / l;
    result.z = si * axis->z / l;
    result.w = co;
    return result;
}

void
quat_muli(Quat *a, const Quat *b)
{
    *a = quat_mul(a, b);
}

Quat
quat_mul(const Quat *a, const Quat *b)
{
    Quat result;
    result.x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;
    result.y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
    result.z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
    result.w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
    return result;
}

Vec3
quat_rotate_vec3(const Quat *q, const Vec3 *v)
{
    Quat p = { v->x, v->y, v->z, 0 };
    Quat q_inv = { -q->x, -q->y, -q->z, q->w };
    p = quat_mul(q, &p);
    quat_muli(&p, &q_inv);
    return { p.x, p.y, p.z };
}
