#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "vector.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    vec3_t velocity;
    float yaw;
    float pitch;
} camera_t;

void camera_init(vec3_t position, vec3_t direction);

vec3_t camera_get_lookat(void);
vec3_t camera_get_position(void);
vec3_t camera_get_direction(void);
vec3_t camera_get_velocity(void);
float  camera_get_yaw(void);
float  camera_get_pitch(void);

void camera_set_position(vec3_t position);
void camera_set_direction(vec3_t direction);
void camera_set_velocity(vec3_t velocity);

void camera_rotate_yaw(float angle);
void camera_rotate_pitch(float angle);


#endif
