#include "camera.h"
#include "clipping.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Global variables for execution status and game loop
bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

// Array to store triangles that should be rendered each frame
#define MAX_TRIANGLES 10000
triangle_t triangles_to_render[MAX_TRIANGLES];
int num_triangles_to_render = 0;

// Declaration of our global transformation matrices
mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;

uint32_t* mesh_texture;
mesh_t mesh = {
    .vertices = {{}},
    .num_vertices = 0,
    .faces = {{}},
    .num_faces = 0,
    .rotation = { 0, 0, 0 },
    .scale = { 1.0, 1.0, 1.0 },
    .translation = { 0, 0, 0 }
};

// Setup function to initialize variables and game objects
void setup()
{
    // Initialize render mode and triangle culling method
    set_render_method(RENDER_TEXTURED);
    set_cull_method(CULL_BACKFACE);

    // Initialize the scene light direction
    init_light(vec3_new(0, 0, 1));

    // Initialize the perspective projection matrix
    float aspect_y = (float)get_window_height() / (float)get_window_width();
    float aspect_x = (float)get_window_width() / (float)get_window_height();
    float fov_y = 3.141592 / 3.0; // the same as 180/3, or 60deg
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
    float znear = 1.0;
    float zfar = 20.0;
    proj_matrix = mat4_make_perspective(fov_y, aspect_y, znear, zfar);

    // Initialize frustum planes with a point and a normal
    init_frustum_planes(fov_x, fov_y, znear, zfar);

    // Loads the vertex and face values for the mesh data structure
    load_obj_file_data(&mesh, "./res/efa.obj");

    // Load the texture information from an external PNG file
    mesh_texture = load_png_texture_data("./res/efa.png");
}

// Poll system events and handle keyboard input
void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
                break;
            }
            if (event.key.keysym.sym == SDLK_1) {
                set_render_method(RENDER_WIRE_VERTEX);
                break;
            }
            if (event.key.keysym.sym == SDLK_2) {
                set_render_method(RENDER_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_3) {
                set_render_method(RENDER_FILL_TRIANGLE);
                break;
            }
            if (event.key.keysym.sym == SDLK_4) {
                set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_5) {
                set_render_method(RENDER_TEXTURED);
                break;
            }
            if (event.key.keysym.sym == SDLK_6) {
                set_render_method(RENDER_TEXTURED_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_c) {
                set_cull_method(CULL_BACKFACE);
                break;
            }
            if (event.key.keysym.sym == SDLK_x) {
                set_cull_method(CULL_NONE);
                break;
            }
            if (event.key.keysym.sym == SDLK_w) {
                camera_set_velocity(vec3_mul(camera_get_direction(), 10.0 * delta_time));
                camera_set_position(vec3_add(camera_get_position(), camera_get_velocity()));
                break;
            }
            if (event.key.keysym.sym == SDLK_s) {
                camera_set_velocity(vec3_mul(camera_get_direction(), 10.0 * delta_time));
                camera_set_position(vec3_sub(camera_get_position(), camera_get_velocity()));
                break;
            }
            if (event.key.keysym.sym == SDLK_a) {
                camera_set_velocity(vec3_mul(vec3_new(-1.0, 0, 0), 10.0 * delta_time));
                camera_set_position(vec3_add(camera_get_position(), camera_get_velocity()));

                break;
            }
            if (event.key.keysym.sym == SDLK_d) {
                camera_set_velocity(vec3_mul(vec3_new(1.0, 0, 0), 10.0 * delta_time));
                camera_set_position(vec3_add(camera_get_position(), camera_get_velocity()));

                break;
            }
            break;
        }
    }
}

// Update function frame by frame with a fixed time step
void update()
{
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // Get a delta time factor converted to seconds to be used to update our game objects
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

    previous_frame_time = SDL_GetTicks();

    // Initialize the counter of triangles to render for the current frame
    num_triangles_to_render = 0;

    // Change the mesh scale, rotation, and translation values per animation frame
    mesh.rotation.x -= 0.2 * delta_time;
    mesh.rotation.y -= 0.2 * delta_time;
    mesh.rotation.z += 0.0 * delta_time;
    mesh.translation.z = 5.0;

    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    // Update camera look at target to create view matrix
    vec3_t target = camera_get_lookat();
    vec3_t up_direction = vec3_new(0, 1, 0);
    view_matrix = mat4_look_at(camera_get_position(), target, up_direction);

    // Loop all triangle faces of our mesh
    for (uint32_t i = 0; i < mesh.num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a World Matrix combining scale, rotation, and translation matrices
            world_matrix = mat4_identity();

            // Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Multiply the view matrix by the vector to transform the scene to camera space
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // Get individual vectors from A, B, and C vertices to compute normal
        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

        // Get the vector subtraction of B-A and C-A
        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        // Compute the face normal (using cross product to find perpendicular)
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);

        // Find the vector between vertex A in the triangle and the camera origin
        vec3_t origin = { 0, 0, 0 };
        vec3_t camera_ray = vec3_sub(origin, vector_a);

        // Calculate how aligned the camera ray is with the face normal (using dot product)
        float dot_normal_camera = vec3_dot(normal, camera_ray);

        // Backface culling test to see if the current face should be projected
        if (should_cull_backface()) {
            // Backface culling, bypassing triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        // Create a polygon from the original transformed triangle to be clipped
        polygon_t polygon = polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv);

        // Clip the polygon and returns a new polygon with potential new vertices
        clip_polygon(&polygon);

        // Break the clipped polygon apart back into individual triangles
        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;

        triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

        // Loops all the assembled triangles after clipping
        for (int t = 0; t < num_triangles_after_clipping; t++) {
            triangle_t triangle_after_clipping = triangles_after_clipping[t];

            vec4_t projected_points[3];

            // Loop all three vertices to perform projection and conversion to screen space
            for (int j = 0; j < 3; j++) {
                // Project the current vertex using a perspective projection matrix
                projected_points[j] = mat4_mul_vec4(proj_matrix, triangle_after_clipping.points[j]);

                // Perform perspective divide
                if (projected_points[j].w != 0) {
                    projected_points[j].x /= projected_points[j].w;
                    projected_points[j].y /= projected_points[j].w;
                    projected_points[j].z /= projected_points[j].w;
                }

                // Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values grow top->down
                projected_points[j].y *= -1;

                // Scale into the view
                projected_points[j].x *= (get_window_width() / 2.0);
                projected_points[j].y *= (get_window_height() / 2.0);

                // Translate the projected points to the middle of the screen
                projected_points[j].x += (get_window_width() / 2.0);
                projected_points[j].y += (get_window_height() / 2.0);
            }

            // Calculate the shade intensity based on how aliged is the normal with the flipped light direction ray
            float light_intensity_factor = -vec3_dot(normal, get_light_direction());

            // Calculate the triangle color based on the light angle
            uint32_t triangle_color = apply_light_intensity(mesh_face.color, light_intensity_factor);

            // Create the final projected triangle that will be rendered in screen space
            triangle_t triangle_to_render = {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
                },
                .texcoords = {
                    { triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v },
                    { triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v },
                    { triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v },
                },
                .color = triangle_color
            };

            // Save the projected triangle in the array of triangles to render
            if (num_triangles_to_render < MAX_TRIANGLES) {
                triangles_to_render[num_triangles_to_render++] = triangle_to_render;
            }
        }
    }
}

// Render function to draw objects on the display
void render()
{
    // Clear all the arrays to get ready for the next frame
    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    draw_grid();

    // Loop all projected triangles and render them
    for (int i = 0; i < num_triangles_to_render; i++) {
        triangle_t triangle = triangles_to_render[i];

        // Draw filled triangle
        if (should_render_filled_triangle()) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // vertex C
                triangle.color);
        }

        // Draw textured triangle
        if (should_render_textured_triangle()) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, // vertex C
                mesh_texture);
        }

        // Draw triangle wireframe
        if (should_render_wire()) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y, // vertex A
                triangle.points[1].x, triangle.points[1].y, // vertex B
                triangle.points[2].x, triangle.points[2].y, // vertex C
                0xFFFFFFFF);
        }

        // Draw triangle vertex points
        if (should_render_wire_vertex()) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFF0000FF); // vertex A
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFF0000FF); // vertex B
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFF0000FF); // vertex C
        }
    }

    // Finally draw the color buffer to the SDL window
    render_color_buffer();
}

int main()
{
    is_running = init_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
