#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vec2 {
    float x, y;
} Vec2;

typedef struct Vec3 {
    float x, y, z;
} Vec3;

//
// Vec2
//
float vec2_length(Vec2 v);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 v, float factor);
Vec2 vec2_div(Vec2 v, float factor);

//
// Vec3
//
float vec3_length(Vec3 v);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_mul(Vec3 v, float factor);
Vec3 vec3_div(Vec3 v, float factor);
Vec3 vec3_cross(Vec3 a, Vec3 b);
float vec3_dot(Vec3 a, Vec3 b);
void vec3_normalize(Vec3 *v);

Vec3 vec3_rotate_x(Vec3 v, float angle);
Vec3 vec3_rotate_y(Vec3 v, float angle);
Vec3 vec3_rotate_z(Vec3 v, float angle);

#endif
