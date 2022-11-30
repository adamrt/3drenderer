#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

typedef struct {
    float m[4][4];
} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
mat4_t mat4_make_rotation_z(float angle);
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);
glm::vec4 mat4_mul_vec4(mat4_t m, glm::vec4 v);
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);
mat4_t mat4_look_at(glm::vec3 eye, glm::vec3 target, glm::vec3 up);
