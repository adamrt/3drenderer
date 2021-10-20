#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

typedef struct mesh_t {
    vec3_t *vertices;   // dynamic array
    face_t *faces;      // dynamic array

    vec3_t rotation;    // rotation with x, y, z values
    vec3_t scale;       // scale with x, y, z values
    vec3_t translation; // trans with x, y, z values
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(char *filename);

#endif
