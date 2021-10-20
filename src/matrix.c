#include <math.h>
#include "matrix.h"

Mat4 mat4_identity(void) {
    Mat4 m = {{
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        }};
    return m;
}

Mat4 mat4_make_scale(float x, float y, float z) {
    Mat4 mat4 = mat4_identity();
    mat4.m[0][0] = x;
    mat4.m[1][1] = y;
    mat4.m[2][2] = z;
    return mat4;
}

Mat4 mat4_make_translation(float x, float y, float z) {
    Mat4 mat4 = mat4_identity();
    mat4.m[0][3] = x;
    mat4.m[1][3] = y;
    mat4.m[2][3] = z;
    return mat4;
}

Mat4 mat4_make_rotation_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    Mat4 m = mat4_identity();
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}

Mat4 mat4_make_rotation_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    Mat4 m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;

}
Mat4 mat4_make_rotation_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    Mat4 m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m;
}

Mat4 mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
    Mat4 m = {{{ 0 }}};
    m.m[0][0] = aspect * (1 / tan(fov /2));
    m.m[1][1] = 1 / tan(fov /2);
    m.m[2][2] = zfar / (zfar - znear);
    m.m[2][3] = (-zfar * znear) / (zfar - znear);
    m.m[3][2] = 1.0;
    return m;
}

Vec4 mat4_mul_vec4(Mat4 m, Vec4 v) {
    Vec4 result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

Mat4 mat4_mul_mat4(Mat4 a, Mat4 b) {
    Mat4 m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }
    return m;
}

Vec4 mat4_mul_vec4_project(Mat4 mat_proj, Vec4 v) {
    Vec4 result = mat4_mul_vec4(mat_proj, v);

    if (result.w != 0.0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    return result;
}
