#include <stdio.h>
#include "mesh.h"
#include "array.h"

Mesh mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 }
};

Vec3 cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1},
    { .x = -1, .y =  1, .z = -1},
    { .x =  1, .y =  1, .z = -1},
    { .x =  1, .y = -1, .z = -1},
    { .x =  1, .y =  1, .z =  1},
    { .x =  1, .y = -1, .z =  1},
    { .x = -1, .y =  1, .z =  1},
    { .x = -1, .y = -1, .z =  1},
};

Face cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3},
    { .a = 1, .b = 3, .c = 4},
    // right
    { .a = 4, .b = 3, .c = 5},
    { .a = 4, .b = 5, .c = 6},
    // back
    { .a = 6, .b = 5, .c = 7},
    { .a = 6, .b = 7, .c = 8},
    // left
    { .a = 8, .b = 7, .c = 2},
    { .a = 8, .b = 2, .c = 1},
    // top
    { .a = 2, .b = 7, .c = 5},
    { .a = 2, .b = 5, .c = 3},
    // bottom
    { .a = 6, .b = 8, .c = 1},
    { .a = 6, .b = 1, .c = 4},
};

void load_cube_mesh_data(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        Vec3 v = cube_vertices[i];
        array_push(mesh.vertices, v);
    }
    for (int i = 0; i < N_CUBE_FACES; i++) {
        Face f = cube_faces[i];
        array_push(mesh.faces, f);
    }
}
