#ifndef CLIPPING_H
#define CLIPPING_H

#include "triangle.h"
#include <glm/vec3.hpp>

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

typedef struct {
    glm::vec3 vertices[MAX_NUM_POLY_VERTICES];
    tex2_t texcoords[MAX_NUM_POLY_VERTICES];
    int num_vertices;
} polygon_t;

void init_frustum_planes(float fov_x, float fov_y, float znear, float zfar);
polygon_t polygon_from_triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, tex2_t t0, tex2_t t1, tex2_t t2);
void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles);
void clip_polygon(polygon_t* polygon);

#endif
