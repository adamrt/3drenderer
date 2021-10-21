#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
    Vec3 position;
    Vec3 direction;
} Camera;

extern Camera camera;

#endif
