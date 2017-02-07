#include "matrix.h"

#include <stddef.h>
#include <math.h>
#include <stdio.h>

#define M4_IDX(ROW, COL) ((COL) * 4 + (ROW))


const Mat4 Mat4_Identity = {{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
}};

const Mat4 Mat4_Zero = {{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
}};


Mat4
mat4_mul(const Mat4 *a, const Mat4 *b)
{
    Mat4 result;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            result.entries[M4_IDX(i, j)] = 0;
            for (size_t k = 0; k < 4; ++k) {
                result.entries[M4_IDX(i, j)] +=
                    a->entries[M4_IDX(i, k)] * b->entries[M4_IDX(k, j)];
            }
        }
    }
    return result;
}

void
mat4_muli(Mat4 *a, const Mat4 *b)
{
    *a = mat4_mul(a, b);
}


Mat4
mat4_transpose(const Mat4 *m)
{
    Mat4 result = *m;
    mat4_transposei(&result);
    return result;
}

void
mat4_transposei(Mat4 *m)
{
    for (size_t row = 0; row < 4; ++row) {
        for (size_t col = row + 1; col < 4; ++col) {
            GLfloat tmp = m->entries[M4_IDX(row, col)];
            m->entries[M4_IDX(row, col)] = m->entries[M4_IDX(col, row)];
            m->entries[M4_IDX(col, row)] = tmp;
        }
    }
}


Mat4
mat4_rotation_x(GLfloat radians)
{
    GLfloat c = cos(radians), s = sin(radians);
    return Mat4 {{
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1,
    }};
}

Mat4
mat4_rotation_y(GLfloat radians)
{
    GLfloat c = cos(radians), s = sin(radians);
    return Mat4 {{
        c, 0, -s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1,
    }};
}

Mat4
mat4_rotation_z(GLfloat radians)
{
    GLfloat c = cos(radians), s = sin(radians);
    return Mat4 {{
        c, s, 0, 0,
        -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    }};
}

Mat4
mat4_translation(GLfloat x, GLfloat y, GLfloat z)
{
    return Mat4 {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1,
    }};
}


void
mat4_print(const Mat4 *m)
{
    printf("%5f %5f %5f %5f\n"
           "%5f %5f %5f %5f\n"
           "%5f %5f %5f %5f\n"
           "%5f %5f %5f %5f\n",
           m->entries[0], m->entries[1], m->entries[2], m->entries[3],
           m->entries[4], m->entries[5], m->entries[6], m->entries[7],
           m->entries[8], m->entries[9], m->entries[10], m->entries[11],
           m->entries[12], m->entries[13], m->entries[14], m->entries[15]);
}


Vec4
mat4_lmul_vec(const Mat4 *m, const Vec4 *v)
{
#define EXTRACT_ROW(R) (Vec4                                          \
        {m->entries[M4_IDX(R, 0)], m->entries[M4_IDX(R, 1)],          \
         m->entries[M4_IDX(R, 2)], m->entries[M4_IDX(R, 3)]})

    Vec4 result, row;

    row = EXTRACT_ROW(0);
    result.x = vec4_dot(&row, v);
    row = EXTRACT_ROW(1);
    result.y = vec4_dot(&row, v);
    row = EXTRACT_ROW(2);
    result.z = vec4_dot(&row, v);
    row = EXTRACT_ROW(3);
    result.w = vec4_dot(&row, v);

    return result;
}
