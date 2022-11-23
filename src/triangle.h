#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdint.h>

#include "texture.h"

struct Vertex {
    glm::vec3 point;
    glm::vec2 uv;
};

struct Face {
    Vertex a, b, c;
    uint32_t color;
};

struct Triangle {
    glm::vec4 points[3];
    glm::vec2 uvs[3];
    uint32_t color;
};

#endif
