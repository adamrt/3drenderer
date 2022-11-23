#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "texture.h"
#include <glm/vec4.hpp>
#include <stdint.h>

typedef struct {
    int a;
    int b;
    int c;
    glm::vec2 a_uv;
    glm::vec2 b_uv;
    glm::vec2 c_uv;
    uint32_t color;
} face_t;

typedef struct {
    glm::vec4 points[3];
    glm::vec2 texcoords[3];
    uint32_t color;
} triangle_t;

#endif
