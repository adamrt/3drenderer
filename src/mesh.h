#ifndef MESH_H
#define MESH_H

#include <stdio.h>
#include "vector.h"
#include "triangle.h"

typedef struct Mesh {
    Vec3 *vertices; // dynamic array
    Face *faces;    // dynamic array
    Vec3 rotation;  // rotation w/ x, y, z values
} Mesh;

extern Mesh mesh;

void load_obj_file(char *filename);

#endif
