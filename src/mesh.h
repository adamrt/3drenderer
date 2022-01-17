#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "vector.h"

#define MAX_TEX_TRIS 512
#define MAX_UNTEX_TRIS 64
#define MAX_TEX_QUADS 768
#define MAX_UNTEX_QUADS 256

#define TOTAL_TRIS MAX_TEX_TRIS + MAX_UNTEX_TRIS
#define TOTAL_QUADS MAX_TEX_QUADS + MAX_UNTEX_QUADS

#define MAX_TRIS TOTAL_TRIS + 2 * TOTAL_QUADS
#define MAX_VERTS TOTAL_TRIS * 3 * 3

// Define a struct for dynamic size meshes, with array of vertices and faces
typedef struct {
    vec3_t vertices[MAX_VERTS];
    uint32_t num_vertices;
    face_t faces[MAX_TRIS];
    uint32_t num_faces;
    vec3_t rotation;    // rotation with x, y, and z values
    vec3_t scale;       // scale with x, y, and z values
    vec3_t translation; // translation with x, y, and z values
} mesh_t;

extern mesh_t mesh;

void load_obj_file_data(char* filename);

#endif
