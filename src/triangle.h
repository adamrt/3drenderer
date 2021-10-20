#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "texture.h"
#include "vector.h"

// Face stores the _indexes_ of the vertex.
typedef struct {
    int a, b, c;
    Tex2 a_uv, b_uv, c_uv;
    uint32_t color;
} Face;

// Triangle stores the vec2 points for the projected triangle.
typedef struct {
    Vec4 points[3];
    Tex2 texcoords[3];
    uint32_t color;
} Triangle;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_triangle_pixel(
    int x, int y,
    uint32_t color,
    Vec4 point_a, Vec4 point_b, Vec4 point_c
);

void draw_filled_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
);

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t* texture
);

#endif
