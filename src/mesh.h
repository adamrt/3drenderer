#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "triangle.h"

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
    std::vector<Face> faces;
    glm::vec3 rotation;    // rotation with x, y, and z values
    glm::vec3 scale;       // scale with x, y, and z values
    glm::vec3 translation; // translation with x, y, and z values
} mesh_t;

void load_obj_file_data(mesh_t* mesh, std::string filename);
