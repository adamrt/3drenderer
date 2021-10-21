#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
    Vec3 position;
    Vec3 direction;
    Vec3 forward_velocity;
    float yaw;
} Camera;

extern Camera camera;

#endif
