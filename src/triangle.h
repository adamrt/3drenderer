#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

// face_t stores the _indexes_ of the vertex.
typedef struct face_t {
    int a;
    int b;
    int c;
} face_t;

// triangle_t stores the vec2 points for the projected triangle.
typedef struct triangle_t {
    vec2_t points[3];
} triangle_t;

#endif
