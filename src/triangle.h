#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

// face_t stores the _indexes_ of the vertex.
typedef struct face_t {
    int a, b, c;
    tex2_t a_uv, b_uv, c_uv;
    uint32_t color;
} face_t;

// triangle_t stores the vec2 points for the projected triangle.
typedef struct triangle_t {
    vec4_t points[3];
    tex2_t texcoords[3];
    uint32_t color;
    float avg_depth;
} triangle_t;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t* texture
);

void sort_triangles_by_z(triangle_t *tris);

#endif
