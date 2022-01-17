#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "vector.h"

// Define a struct for dynamic size meshes, with array of vertices and faces
typedef struct {
    vec3_t* vertices;   // dynamic array of vertices
    face_t* faces;      // dynamic array of faces
    vec3_t rotation;    // rotation with x, y, and z values
    vec3_t scale;       // scale with x, y, and z values
    vec3_t translation; // translation with x, y, and z values
} mesh_t;

extern mesh_t mesh;

void load_obj_file_data(char* filename);

#endif
