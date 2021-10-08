#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec2_t {
    float x;
    float y;
} vec2_t;

typedef struct vec3_t {
    float x;
    float y;
    float z;
} vec3_t;


vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

#endif
