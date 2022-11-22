#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>

#include "matrix.h"

typedef struct {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 velocity;
    float yaw;
    float pitch;
} camera_t;

void camera_init(glm::vec3 position, glm::vec3 direction);

glm::vec3 camera_get_lookat(void);
glm::vec3 camera_get_position(void);
glm::vec3 camera_get_direction(void);
glm::vec3 camera_get_velocity(void);
float camera_get_yaw(void);
float camera_get_pitch(void);

void camera_set_position(glm::vec3 position);
void camera_set_direction(glm::vec3 direction);
void camera_set_velocity(glm::vec3 velocity);

void camera_rotate_yaw(float angle);
void camera_rotate_pitch(float angle);

#endif
