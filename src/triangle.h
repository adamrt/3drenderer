#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "texture.h"
#include <glm/vec4.hpp>
#include <stdint.h>

typedef struct {
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t;

typedef struct {
    glm::vec4 points[3];
    tex2_t texcoords[3];
    uint32_t color;
} triangle_t;

#endif
