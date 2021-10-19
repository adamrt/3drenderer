#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "texture.h"
#include "vector.h"

// face_t stores the _indexes_ of the vertex.
typedef struct face_t {
    int a, b, c;
    tex2_t a_uv, b_uv, c_uv;
    uint32_t color;
} face_t;

// triangle_t stores the vec2 points for the projected triangle.
typedef struct triangle_t {
    vec2_t points[3];
    tex2_t texcoords[3];
    uint32_t color;
    float avg_depth;
} triangle_t;

void sort_triangles_by_z(triangle_t *tris);

#endif
