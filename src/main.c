#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "array.h"
#include "camera.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "options.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Triangle array to render each frame
#define MAX_TRIANGLES_PER_MESH 10000
Triangle triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

Mat4 world_matrix;
Mat4 proj_matrix;
Mat4 view_matrix;

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

Options options;

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
    options.enable_backface_culling = true;
    options.enable_wireframe = false;
    options.enable_fill_triangles = false;
    options.enable_textured_triangles = true;

    color_buffer = malloc(sizeof(uint32_t) * window_width * window_height);
    if (!color_buffer) {
        fprintf(stderr, "Error allocating for color_buffer\n");
        return false;
    }

    z_buffer = malloc(sizeof(float) * window_width * window_height);
    if (!z_buffer) {
        fprintf(stderr, "Error allocating for z_buffer\n");
        return false;
    }

    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    if (!color_buffer_texture) {
        fprintf(stderr, "Error creating texture\n");
        return false;
    }

    float fov = M_PI / 3.0; // 180/3 or 60deg
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    // load_cube_mesh_data();
    load_obj_file_data("res/efa.obj");
    load_png_texture_data("./res/efa.png");

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
            if (options.enable_fill_triangles) {
                options.enable_textured_triangles = false;
            }
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
    // Get a delta time factor converted to seconds to be used to update our game objects
    int ms_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (ms_to_wait > 0) {
        SDL_Delay(ms_to_wait);
    }

    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

    previous_frame_time = SDL_GetTicks();

    num_triangles_to_render = 0;

    // Change mesh scale, rotation and translation values per frame
    mesh.rotation.x += 0.0 * delta_time;
    mesh.rotation.y += 0.0 * delta_time;
    mesh.rotation.z += 0.0 * delta_time;
    mesh.translation.z = 5.0;

    // Change the camera position per frame
    camera.position.x += 0.03;
    camera.position.y += 0.03;

    // Create the view matrix looking at a hardcoded point
    Vec3 target = { 0, 0, 4.0 };
    Vec3 up = { 0, 1, 0 };
    view_matrix = mat4_look_at(camera.position, target, up);

    Mat4 scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    Mat4 rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    Mat4 rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    Mat4 rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
    Mat4 translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);


    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        Face mesh_face = mesh.faces[i];

        Vec3 face_vertices[3];

        // Offset for 0 vs 1-index
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        Vec4 transformed_vertices[3];

        // Tranform
        for (int j = 0; j < 3; j++) {
            Vec4 transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Matrix for scale, rotation and translation.
            // Order matters
            world_matrix = mat4_identity();

            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Multiply view matrix by the vector to transform the scene to camera space
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // Backface culling
        Vec3 vector_a = vec3_from_vec4(transformed_vertices[0]);
        Vec3 vector_b = vec3_from_vec4(transformed_vertices[1]);
        Vec3 vector_c = vec3_from_vec4(transformed_vertices[2]);

        Vec3 vector_ab = vec3_sub(vector_b, vector_a);
        Vec3 vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        // Face Normal
        Vec3 normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);

        Vec3 origin = { 0, 0, 0 };
        Vec3 camera_ray = vec3_sub(origin, vector_a);
        float dot_normal_camera = vec3_dot(normal, camera_ray);

        // Skip triangles that are looking away from camera.  Calculate backface culling
        // above as we need the normal calculation below for lighting.
        if (options.enable_backface_culling) {
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        // Projection
        Vec4 projected_points[3];

        for (int j = 0; j < 3; j++) {
            projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

            // Scale into the view
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height/ 2.0);

            // Invert the y value to account for flipped y coordinate.  TODO: Does this
            // only matter for OBJ files? Should it be handled in OBJ load function so
            // other loading function to suffer.
            projected_points[j].y *= -1;

            // Translate the projected point to the middle of the screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        float light_intensity_factor = -vec3_dot(normal, light.direction);
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

        Triangle projected_triangle = {
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
        };


        // Save the projected triangle into the triangles to render.
        if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
            triangles_to_render[num_triangles_to_render] = projected_triangle;
            num_triangles_to_render++;
        }
    }
}

void render(void) {
    draw_grid(0xFF333333);

    for (int i = 0; i < num_triangles_to_render; i++) {
        Triangle triangle = triangles_to_render[i];

        if (options.enable_textured_triangles) {
            draw_textured_triangle(
                                   triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                                   triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                                   triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                                   mesh_texture);
        }

        if (options.enable_fill_triangles) {
            draw_filled_triangle(
                                 triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                                 triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                                 triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                                 triangle.color);
        }

        if (options.enable_wireframe) {
            draw_triangle(
                          triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          0xFFFFFFFF);
        }

    }


    render_color_buffer();

    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    free(z_buffer);
    upng_free(png_texture);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}
