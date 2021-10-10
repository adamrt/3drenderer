#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include <stdlib.h>
#include "vector.h"

// Face stores the _indexes_ of the vertex.
typedef struct Face {
    int a, b, c;
    uint32_t color;
} Face;

// Triangle stores the vec2 points for the projected triangle.
typedef struct Triangle {
    Vec2 points[3];
    uint32_t color;
} Triangle;

#endif
