#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

typedef struct {
    Vec3 *vertices;   // dynamic array
    Face *faces;      // dynamic array

    Vec3 rotation;    // rotation with x, y, z values
    Vec3 scale;       // scale with x, y, z values
    Vec3 translation; // trans with x, y, z values
} Mesh;

extern Mesh mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(char *filename);

#endif
