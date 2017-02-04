#ifndef GARDEN_MATRIX_HEADER_INCLUDED
#define GARDEN_MATRIX_HEADER_INCLUDED

#include "gl.h"


typedef struct Mat4 {
    GLfloat entries[16];
} Mat4;

extern const Mat4 Mat4_Identity;
extern const Mat4 Mat4_Zero;


Mat4 mat4_mul(const Mat4 *a, const Mat4 *b);
void mat4_muli(Mat4 *a, const Mat4 *b);

Mat4 mat4_transpose(const Mat4 *m);
void mat4_transposei(Mat4 *m);

Mat4 mat4_rotation_x(GLfloat radians);
Mat4 mat4_rotation_y(GLfloat radians);
Mat4 mat4_rotation_z(GLfloat radians);

void mat4_print(const Mat4 *m);

#endif
