#include <glm/vec2.hpp>
#include <stdio.h>
#include <string.h>

#include "mesh.h"

void load_obj_file_data(mesh_t* mesh, std::string filename)
{
    FILE* file;
    file = fopen(filename.c_str(), "r");
    char line[1024];

    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> vertices;

    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            glm::vec3 vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        // Texture coordinate information
        if (strncmp(line, "vt ", 3) == 0) {
            glm::vec2 uv;
            sscanf(line, "vt %f %f", &uv.x, &uv.y);
            uvs.push_back(uv);
        }
        // Face information
        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]);

            Face face = {
                .a = {
                    .point = vertices[vertex_indices[0] - 1],
                    .uv = uvs[texture_indices[0] - 1],
                },
                .b = {
                    .point = vertices[vertex_indices[1] - 1],
                    .uv = uvs[texture_indices[1] - 1],
                },
                .c = {
                    .point = vertices[vertex_indices[2] - 1],
                    .uv = uvs[texture_indices[2] - 1],
                },
                .color = 0xFFFFFFFF
            };
            mesh->faces.push_back(face);
        };
    }
    fclose(file);
}
