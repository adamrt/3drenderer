#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdlib.h>
#include "vector.h"

// Face stores the _indexes_ of the vertex.
typedef struct Face {
    int a, b, c;
} Face;

// Triangle stores the vec2 points for the projected triangle.
typedef struct Triangle {
    Vec2 points[3];
} Triangle;

// TriangleArray is a dynamic array of Triangles.
typedef struct TriangleArray {
  Triangle *triangles;
  size_t capacity;
  size_t length;
} TriangleArray;

void triangle_array_init(TriangleArray *a, size_t initial_size);
void triangle_array_insert(TriangleArray *a, Triangle triangle);
void triangle_array_free(TriangleArray *a);

#endif
