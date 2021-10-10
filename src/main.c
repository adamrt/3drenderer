#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "array.h"
#include "display.h"
#include "mesh.h"
#include "options.h"
#include "triangle.h"
#include "vector.h"


triangle_t *triangles_to_render = NULL;
vec3_t camera_position = { 0, 0, 0 };

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

options_t options;

bool setup(void);
void process_input(void);
void update(void);
void render(void);
void free_resources(void);
void sort_triangles_by_z(triangle_t *tris);

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
    free_resources();

    return 0;
}

bool setup(void) {
    options = new_options();

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

    load_cube_mesh_data();
    // load_obj_file("res/cube.obj");

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
        if (event.key.keysym.sym == SDLK_1) {
            options.enable_backface_culling = !options.enable_backface_culling;
        }
        if (event.key.keysym.sym == SDLK_2) {
            options.enable_wireframe = !options.enable_wireframe;
        }
        if (event.key.keysym.sym == SDLK_3) {
            options.enable_fill_triangles = !options.enable_fill_triangles;
        }
        break;
    }
}


vec2_t project(vec3_t point) {
    vec2_t projected_point = {
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

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;


    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];

        // Offset for 0 vs 1-index
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];

        // Tranform
        for (int j = 0; j < 3; j++) {
            vec3_t vertex = face_vertices[j];

            // Rotate the vertex
            vec3_t transformed_vertex;
            transformed_vertex = vec3_rotate_x(vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // Translate the vertex away from the camera in z
            transformed_vertex.z += 5;

            // Save vertices in the array of vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // Backface culling
        if (options.enable_backface_culling) {
            vec3_t vec_a = transformed_vertices[0];
            vec3_t vec_b = transformed_vertices[1];
            vec3_t vec_c = transformed_vertices[2];

            vec3_t vec_ab = vec3_sub(vec_b, vec_a);
            vec3_t vec_ac = vec3_sub(vec_c, vec_a);
            vec3_normalize(&vec_ab);
            vec3_normalize(&vec_ac);

            // face_t Normal
            vec3_t normal = vec3_cross(vec_ab, vec_ac);
            vec3_normalize(&normal);

            vec3_t camera_ray = vec3_sub(camera_position, vec_a);
            float dot_normal_camera = vec3_dot(normal, camera_ray);

            // Skip triangles that are looking away from camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        // Projection
        vec2_t projected_points[3];
        for (int j = 0; j < 3; j++) {
            projected_points[j] = project(transformed_vertices[j]);

            // Scale and translate the projected point to the middle of the screen
            projected_points[j].x += (SCREEN_WIDTH / 2);
            projected_points[j].y += (SCREEN_HEIGHT / 2);
        }

        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y },
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth,
        };

        // Save the projected triangle into the triangles to render.
        array_push(triangles_to_render, projected_triangle);
    }

    sort_triangles_by_z(triangles_to_render);
}

void render(void) {
    draw_grid(0xFF333333);

    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        if (options.enable_fill_triangles) {
            draw_filled_triangle(
                                 triangle.points[0].x, triangle.points[0].y,
                                 triangle.points[1].x, triangle.points[1].y,
                                 triangle.points[2].x, triangle.points[2].y,
                                 triangle.color);
        }

        if (options.enable_wireframe) {
            draw_triangle(
                          triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          0xFFFFFF00);
        }

    }


    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

// TODO: this is bubble sort-ish. change to merge sort or something faster.
void sort_triangles_by_z(triangle_t *tris) {
   // z gets higher further into the screen (left handed);
   // higher numbers should be first
   int num_tris = array_length(tris);
    while(true) {
        bool has_changed = false;
        // use num_tris - 1 because we use i+1 for b;
        for (int i = 0; i < (num_tris - 1); i++) {
            if (tris[i].avg_depth < tris[i+1].avg_depth) {
                triangle_t tmp = tris[i];
                tris[i] = tris[i+1];
                tris[i+1]  = tmp;
                has_changed = true;
            }
        }
        if (has_changed == false) {
            break;
        }
    }
}
