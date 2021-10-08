#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"

TriangleArray triangles_to_render;
Vec3 camera_position = { 0, 0, -5 };
Vec3 cube_rotation = { 0, 0, 0 };

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

bool setup(void);
void process_input(void);
void update(void);
void render(void);

int main(void){
    if (!initialize_window()) {
        return 1;
    };

    if (!setup()) {
        return 1;
    };

    is_running = true;

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}

bool setup(void) {
    color_buffer = malloc((int)sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
    if (!color_buffer) {
        fprintf(stderr, "Error allocating for color_buffer\n");
        return false;
    }

    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!color_buffer_texture) {
        fprintf(stderr, "Error creating texture\n");
        return false;
    }

    return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type){
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            is_running = false;
        }
        break;
    }
}


Vec2 project(Vec3 point) {
    Vec2 projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };
    return projected_point;
}

void update(void) {
    int ms_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (ms_to_wait > 0) {
        SDL_Delay(ms_to_wait);
    }
    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    triangle_array_init(&triangles_to_render, 2);

    for (int i = 0; i < N_MESH_FACES; i++) {
        Face mesh_face = mesh_faces[i];

        Vec3 face_vertices[3];

        // Offset for 0 vs 1-index
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        Triangle projected_triangle;

        for (int j = 0; j < 3; j++) {
            Vec3 vertex = face_vertices[j];

            // Rotate the vertex
            Vec3 transformed_vertex;
            transformed_vertex = vec3_rotate_x(vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            // Translate the vertex away from the camera in z
            transformed_vertex.z -= camera_position.z;

            Vec2 projected_point = project(transformed_vertex);


            // Scale and translate the projected point to the middle of the screen
            projected_point.x += (SCREEN_WIDTH / 2);
            projected_point.y += (SCREEN_HEIGHT / 2);

            projected_triangle.points[j] = projected_point;

        }

        // Save the projected triangle into the triangles to render.
        triangle_array_insert(&triangles_to_render, projected_triangle);
    }
}

void render(void) {
    draw_grid(0xFF333333);

    int num_triangles = triangles_to_render.length;

    for (int i = 0; i < num_triangles; i++) {
        Triangle triangle = triangles_to_render.triangles[i];

        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

        draw_triangle(triangle, 0xFF00FF00);
    }

    // TODO: For implementation simplicity the triangle array memory allocated and freed
    // each frame. A better solution would be to reuse the same allocated memory each
    // frame since the number of triangles doesn't change.
    //
    // Originally that is how the implementation worked, but I was thinking that when we
    // implement backface culling, the number of triangles actually might change, so I
    // reverted to this version until we know.
    triangle_array_free(&triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}
