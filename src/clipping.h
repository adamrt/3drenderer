#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "triangle.h"

#define MAX_NUM_POLY_VERTICES 10
#define MAX_NUM_POLY_TRIANGLES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct {
    glm::vec3 point;
    glm::vec3 normal;
} plane_t;

class Polygon {
public:
    // From triangle constructor;
    Polygon(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 t0, glm::vec2 t1, glm::vec2 t2)
        : vertices { v0, v1, v2 }
        , texcoords { t0, t1, t2 }
        , num_vertices(3) {};

    std::vector<Triangle> clipped_triangles();

private:
    void clip();
    void clip_against_plane(int plane);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texcoords;
    int num_vertices;
};

void init_frustum_planes(float fov_x, float fov_y, float znear, float zfar);
void clip_polygon(Polygon* polygon);
