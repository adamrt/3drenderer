#include <stdio.h>
#include <string.h>
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
    { .a = 1, .b = 2, .c = 3, .color = 0xFFFF00FF },
    { .a = 1, .b = 3, .c = 4, .color = 0xFFFF00FF },
    // right
    { .a = 4, .b = 3, .c = 5, .color = 0xFF00FF00 },
    { .a = 4, .b = 5, .c = 6, .color = 0xFF00FF00 },
    // back
    { .a = 6, .b = 5, .c = 7, .color = 0xFF0000FF },
    { .a = 6, .b = 7, .c = 8, .color = 0xFF0000FF },
    // left
    { .a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00 },
    { .a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00 },
    // top
    { .a = 2, .b = 7, .c = 5, .color = 0xFFFF0000 },
    { .a = 2, .b = 5, .c = 3, .color = 0xFFFF0000 },
    // bottom
    { .a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF },
    { .a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF },
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

void load_obj_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "failed to open %s\n", filename);
        exit(1);
    }

    char line[1024];
    while ((fgets(line, 1000, file))) {
        if (strncmp(line, "v ", 2) == 0) {
            Vec3 vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }
        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];

            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                   &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                   &vertex_indices[2], &texture_indices[2], &normal_indices[2]);

            Face face = {
                .a = vertex_indices[0],
                .b = vertex_indices[1],
                .c = vertex_indices[2],
            };
            array_push(mesh.faces, face);
        }
    }
}
