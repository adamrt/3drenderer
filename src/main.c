#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "options.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

triangle_t *triangles_to_render = NULL;
vec3_t camera_position = { 0, 0, 0 };
mat4_t proj_matrix;

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

options_t options;

bool setup(void);
void process_input(void);
void update(void);
void render(void);
void free_resources(void);

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
    options = options_default();

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

    float fov = M_PI / 3.0; // 180/3 or 60deg
    float aspect = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    texture_width = 64;
    texture_height = 64;

    load_cube_mesh_data();
    // load_obj_file("res/f22.obj");

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
        if (event.key.keysym.sym == SDLK_4) {
            options.enable_textured_triangles = !options.enable_textured_triangles;
            if (options.enable_textured_triangles) {
                options.enable_fill_triangles = false;
            }
        }
        break;
    }
}

void update(void) {
    int ms_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (ms_to_wait > 0) {
        SDL_Delay(ms_to_wait);
    }
    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    mesh.rotation.x += 0.02;
    mesh.rotation.y += 0.02;
    mesh.rotation.z += 0.02;
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;
    // mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);


    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];

        // Offset for 0 vs 1-index
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // Tranform
        for (int j = 0; j < 3; j++) {

            // Matrix for scale, rotation and translation.
            // Order matters
            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Mulitiple the world matrix by the original vector
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        // Backface culling
        vec3_t vec_a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t vec_b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t vec_c = vec3_from_vec4(transformed_vertices[2]);

        vec3_t vec_ab = vec3_sub(vec_b, vec_a);
        vec3_t vec_ac = vec3_sub(vec_c, vec_a);
        vec3_normalize(&vec_ab);
        vec3_normalize(&vec_ac);

        // face_t Normal
        vec3_t normal = vec3_cross(vec_ab, vec_ac);
        vec3_normalize(&normal);

        vec3_t camera_ray = vec3_sub(camera_position, vec_a);
        float dot_normal_camera = vec3_dot(normal, camera_ray);

        // Skip triangles that are looking away from camera.  Calculate backface culling
        // above as we need the normal calculation below for lighting.
        if (options.enable_backface_culling) {
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        // Projection
        vec4_t projected_points[3];

        for (int j = 0; j < 3; j++) {
            projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

            // Scale into the view
            projected_points[j].x *= (SCREEN_WIDTH / 2.0);
            projected_points[j].y *= (SCREEN_HEIGHT/ 2.0);

            // Invert the y value to account for flipped y coordinate.  TODO: Does this
            // only matter for OBJ files? Should it be handled in OBJ load function so
            // other loading function to suffer.
            projected_points[j].y *= -1;

            // Translate the projected point to the middle of the screen
            projected_points[j].x += (SCREEN_WIDTH / 2.0);
            projected_points[j].y += (SCREEN_HEIGHT / 2.0);
        }

        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

        float light_intensity_factor = -vec3_dot(normal, light.direction);
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
            },
            .texcoords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            },
            .color = triangle_color,
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

        if (options.enable_textured_triangles) {
            draw_textured_triangle(
                                   triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                                   triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                                   triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                                   mesh_texture);
        }

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
