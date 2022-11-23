#include "Engine.h"
#include "clipping.h"
#include "matrix.h"
#include "mesh.h"

// Array to store triangles that should be rendered each frame
#define MAX_TRIANGLES 10000
Triangle triangles_to_render[MAX_TRIANGLES];
int num_triangles_to_render = 0;

float delta_time = 0;
int previous_frame_time = 0;

uint32_t* mesh_texture;
mesh_t mesh = {
    .faces = { {} },
    .rotation = { 0, 0, 0 },
    .scale = { 1.0, 1.0, 1.0 },
    .translation = { 0, 0, 0 }
};

// Declaration of our global transformation matrices
mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;

Engine::Engine(int width, int height)
{
    m_fb = new Framebuffer(width, height);
    m_window = new Window(m_fb, width, height);
    m_light = new Light(glm::vec3(0, 0, 1));
    m_camera = new Camera(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0));
};

// Setup function to initialize variables and game objects
void Engine::setup()
{
    // Initialize the scene light direction

    // Initialize the perspective projection matrix
    float aspect_y = (float)m_window->get_height() / (float)m_window->get_width();
    float aspect_x = (float)m_window->get_width() / (float)m_window->get_height();
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
void Engine::process_input()
{

    float sensitivity = 10.0;

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
                m_fb->set_render_method(RenderMethod::WireVertex);
                break;
            }
            if (event.key.keysym.sym == SDLK_2) {
                m_fb->set_render_method(RenderMethod::Wire);
                break;
            }
            if (event.key.keysym.sym == SDLK_3) {
                m_fb->set_render_method(RenderMethod::FillTriangle);
                break;
            }
            if (event.key.keysym.sym == SDLK_4) {
                m_fb->set_render_method(RenderMethod::FillTriangleWire);
                break;
            }
            if (event.key.keysym.sym == SDLK_5) {
                m_fb->set_render_method(RenderMethod::Textured);
                break;
            }
            if (event.key.keysym.sym == SDLK_6) {
                m_fb->set_render_method(RenderMethod::TexturedWire);
                break;
            }
            if (event.key.keysym.sym == SDLK_c) {
                m_fb->set_cull_method(CullMethod::Backface);
                break;
            }
            if (event.key.keysym.sym == SDLK_x) {
                m_fb->set_cull_method(CullMethod::None);
                break;
            }
            if (event.key.keysym.sym == SDLK_w) {
                auto velocity = m_camera->direction * sensitivity * delta_time;
                m_camera->position = m_camera->position + velocity;
                break;
            }
            if (event.key.keysym.sym == SDLK_s) {
                auto velocity = m_camera->direction * sensitivity * delta_time;
                m_camera->position = m_camera->position - velocity;
                break;
            }
            if (event.key.keysym.sym == SDLK_a) {
                auto velocity = glm::vec3(-1.0, 0, 0) * sensitivity * delta_time;
                m_camera->position = m_camera->position + velocity;
                break;
            }
            if (event.key.keysym.sym == SDLK_d) {
                auto velocity = glm::vec3(1.0, 0, 0) * sensitivity * delta_time;
                m_camera->position = m_camera->position + velocity;
                break;
            }
            break;
        }
    }
}

// Update function frame by frame with a fixed time step
void Engine::update()
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
    glm::vec3 target = m_camera->get_look_at();
    glm::vec3 up_direction = glm::vec3(0, 1, 0);
    view_matrix = mat4_look_at(m_camera->position, target, up_direction);

    // Loop all triangle faces of our mesh
    for (uint32_t i = 0; i < mesh.faces.size(); i++) {
        Face mesh_face = mesh.faces[i];

        glm::vec3 face_vertices[3];
        face_vertices[0] = mesh_face.a.point;
        face_vertices[1] = mesh_face.b.point;
        face_vertices[2] = mesh_face.c.point;

        glm::vec4 transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            glm::vec4 transformed_vertex(face_vertices[j], 1);

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
        glm::vec3 vector_a(transformed_vertices[0]); /*   A   */
        glm::vec3 vector_b(transformed_vertices[1]); /*  / \  */
        glm::vec3 vector_c(transformed_vertices[2]); /* C---B */

        // Get the vector subtraction of B-A and C-A
        glm::vec3 vector_ab = vector_b - vector_a;
        glm::vec3 vector_ac = vector_c - vector_a;
        vector_ab = glm::normalize(vector_ab);
        vector_ac = glm::normalize(vector_ac);

        // Compute the face normal (using cross product to find perpendicular)
        glm::vec3 normal = glm::cross(vector_ab, vector_ac);
        normal = glm::normalize(normal);

        // Find the vector between vertex A in the triangle and the camera origin
        glm::vec3 origin = { 0, 0, 0 };
        glm::vec3 camera_ray = origin - vector_a;

        // Calculate how aligned the camera ray is with the face normal (using dot product)
        float dot_normal_camera = glm::dot(normal, camera_ray);

        // Backface culling test to see if the current face should be projected
        if (m_fb->should_cull_backface()) {
            // Backface culling, bypassing triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        // Create a polygon from the original transformed triangle to be clipped
        polygon_t polygon = polygon_from_triangle(
            glm::vec3(transformed_vertices[0]),
            glm::vec3(transformed_vertices[1]),
            glm::vec3(transformed_vertices[2]),
            mesh_face.a.uv,
            mesh_face.b.uv,
            mesh_face.c.uv);

        // Clip the polygon and returns a new polygon with potential new vertices
        clip_polygon(&polygon);

        // Break the clipped polygon apart back into individual triangles
        Triangle triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;

        triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

        // Loops all the assembled triangles after clipping
        for (int t = 0; t < num_triangles_after_clipping; t++) {
            Triangle triangle_after_clipping = triangles_after_clipping[t];

            glm::vec4 projected_points[3];

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
                projected_points[j].x *= (m_window->get_width() / 2.0);
                projected_points[j].y *= (m_window->get_height() / 2.0);

                // Translate the projected points to the middle of the screen
                projected_points[j].x += (m_window->get_width() / 2.0);
                projected_points[j].y += (m_window->get_height() / 2.0);
            }

            // Calculate the triangle color based on the light angle
            uint32_t triangle_color = m_light->calculate_light_color(mesh_face.color, normal);

            // Create the final projected triangle that will be rendered in screen space
            Triangle triangle_to_render = {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
                },
                .uvs = {
                    { triangle_after_clipping.uvs[0].x, triangle_after_clipping.uvs[0].y },
                    { triangle_after_clipping.uvs[1].x, triangle_after_clipping.uvs[1].y },
                    { triangle_after_clipping.uvs[2].x, triangle_after_clipping.uvs[2].y },
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
void Engine::render()
{
    // Clear all the arrays to get ready for the next frame
    m_fb->clear_color(0xFF000000);
    m_fb->clear_depth();

    m_fb->draw_grid();

    // Loop all projected triangles and render them
    for (int i = 0; i < num_triangles_to_render; i++) {
        Triangle triangle = triangles_to_render[i];

        // Draw filled triangle
        if (m_fb->should_render_filled_triangle()) {
            m_fb->draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // vertex C
                triangle.color);
        }

        // Draw textured triangle
        if (m_fb->should_render_textured_triangle()) {
            m_fb->draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.uvs[0].x, triangle.uvs[0].y, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.uvs[1].x, triangle.uvs[1].y, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.uvs[2].x, triangle.uvs[2].y, // vertex C
                mesh_texture);
        }

        // Draw triangle wireframe
        if (m_fb->should_render_wire()) {
            m_fb->draw_triangle(
                triangle.points[0].x, triangle.points[0].y, // vertex A
                triangle.points[1].x, triangle.points[1].y, // vertex B
                triangle.points[2].x, triangle.points[2].y, // vertex C
                0xFFFFFFFF);
        }

        // Draw triangle vertex points
        if (m_fb->should_render_wire_vertex()) {
            m_fb->draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFF0000FF); // vertex A
            m_fb->draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFF0000FF); // vertex B
            m_fb->draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFF0000FF); // vertex C
        }
    }

    // Finally draw the color buffer to the SDL window
    m_window->render();
}
