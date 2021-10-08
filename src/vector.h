#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vec2 {
    float x;
    float y;
} Vec2;

typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;


Vec3 vec3_rotate_x(Vec3 v, float angle);
Vec3 vec3_rotate_y(Vec3 v, float angle);
Vec3 vec3_rotate_z(Vec3 v, float angle);

#endif
