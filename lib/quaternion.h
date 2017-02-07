#ifndef GARDEN_QUATERNION_HEADER_INCLUDED
#define GARDEN_QUATERNION_HEADER_INCLUDED

#include "gl.h"


typedef struct Vec3 Vec3;

typedef struct Quat {
    GLfloat x, y, z, w;
} Quat;


Quat quat_angle_axis(GLfloat angle, Vec3 *axis);

void quat_muli(Quat *a, const Quat *b);
Quat quat_mul(const Quat *a, const Quat *b);

Vec3 quat_rotate_vec3(const Quat *q, const Vec3 *v);


#endif
