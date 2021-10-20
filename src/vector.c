#include <math.h>
#include "vector.h"

// Pythagoras for 2D
float vec2_length(Vec2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

Vec2 vec2_add(Vec2 a, Vec2 b) {
    Vec2 result = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return result;
}

Vec2 vec2_sub(Vec2 a, Vec2 b) {
    Vec2 result = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return result;
}

Vec2 vec2_mul(Vec2 v, float factor) {
    Vec2 result = {
        .x = v.x * factor,
        .y = v.y * factor
    };
    return result;
}

Vec2 vec2_div(Vec2 v, float factor) {
    Vec2 result = {
        .x = v.x / factor,
        .y = v.y / factor
    };
    return result;
}

float vec2_dot(Vec2 a, Vec2 b) {
    return (a.x * b.x) + (a.y * b.y);
}

void vec2_normalize(Vec2 *v) {
    float length = sqrt(v->x * v->x + v->y * v->y);
    v->x /= length;
    v->y /= length;
}

// Pythagoras for 3D
float vec3_length(Vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return result;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return result;
}

Vec3 vec3_mul(Vec3 v, float factor) {
    Vec3 result = {
        .x = v.x * factor,
        .y = v.y * factor,
        .z = v.z * factor
    };
    return result;
}

Vec3 vec3_div(Vec3 v, float factor) {
    Vec3 result = {
        .x = v.x / factor,
        .y = v.y / factor,
        .z = v.z / factor
    };
    return result;
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return result;
}

float vec3_dot(Vec3 a, Vec3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vec3_normalize(Vec3 *v) {
    float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

Vec3 vec3_rotate_x(Vec3 v, float angle) {
    Vec3 rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

Vec3 vec3_rotate_y(Vec3 v, float angle) {
    Vec3 rotated_vector = {
        .x = v.x * cos(angle) + v.z * sin(angle),
        .y = v.y,
        .z = -v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

Vec3 vec3_rotate_z(Vec3 v, float angle) {
    Vec3 rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;
}

//
// Vector conversions
//

Vec4 vec4_from_vec3(Vec3 v) {
    Vec4 result = { v.x, v.y, v.z, 1.0 };
    return result;
}

Vec3 vec3_from_vec4(Vec4 v) {
    Vec3 result = { v.x, v.y, v.z };
    return result;
}

Vec2 vec2_from_vec4(Vec4 v) {
    Vec2 result = { v.x, v.y };
    return result;
}
