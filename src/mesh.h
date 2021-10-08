#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_MESH_VERTICES 8
extern Vec3 mesh_vertices[N_MESH_VERTICES];

#define N_MESH_FACES (6 * 2)
extern Face mesh_faces[N_MESH_FACES];

#endif
