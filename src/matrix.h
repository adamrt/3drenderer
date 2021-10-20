#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
    float m[4][4];
} Mat4;

Mat4 mat4_identity(void);

Mat4 mat4_make_scale(float x, float y, float z);
Mat4 mat4_make_translation(float x, float y, float z);
Mat4 mat4_make_rotation_x(float angle);
Mat4 mat4_make_rotation_y(float angle);
Mat4 mat4_make_rotation_z(float angle);
Mat4 mat4_make_perspective(float fov, float aspect, float znear, float zfar);

Vec4 mat4_mul_vec4(Mat4 m, Vec4 v);
Mat4 mat4_mul_mat4(Mat4 a, Mat4 b);
Vec4 mat4_mul_vec4_project(Mat4 mat_proj, Vec4 v);

#endif
