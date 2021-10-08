#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

// Face stores the _indexes_ of the vertex.
typedef struct Face {
    int a;
    int b;
    int c;
} Face;

// Triangle stores the vec2 points for the projected triangle.
typedef struct Triangle {
    Vec2 points[3];
} Triangle;

#endif
