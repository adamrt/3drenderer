#ifndef MESH_H
#define MESH_H

#include <stdio.h>
#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

extern Vec3 cube_vertices[N_CUBE_VERTICES];
extern Face cube_faces[N_CUBE_FACES];

typedef struct Mesh {
    Vec3 *vertices; // dynamic array
    Face *faces;    // dynamic array
    Vec3 rotation;  // rotation w/ x, y, z values
} Mesh;

extern Mesh mesh;

void load_cube_mesh_data(void);

#endif
