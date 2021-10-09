#include <stdio.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

Mesh mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 }
};

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
