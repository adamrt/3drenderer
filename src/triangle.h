#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include <stdlib.h>
#include "vector.h"

// face_t stores the _indexes_ of the vertex.
typedef struct face_t {
    int a, b, c;
    uint32_t color;
} face_t;

// triangle_t stores the vec2 points for the projected triangle.
typedef struct triangle_t {
    vec2_t points[3];
    uint32_t color;
    float avg_depth;
} triangle_t;

#endif
