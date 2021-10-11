#ifndef MESH_H
#define MESH_H

#include <stdio.h>
#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

typedef struct mesh_t {
    vec3_t *vertices;   // dynamic array
    face_t *faces;      // dynamic array

    vec3_t rotation;    // rotation with x, y, z values
    vec3_t scale;       // scale with x, y, z values
    vec3_t translation; // trans with x, y, z values
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file(char *filename);

#endif
