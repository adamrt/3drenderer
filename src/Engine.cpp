#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine.h"
#include "clipping.h"
#include "mesh.h"

std::vector<Triangle> triangles_to_render;

uint32_t* mesh_texture;
mesh_t mesh = {
    .faces = { {} },
    .rotation = { 0, 0, 0 },
    .scale = { 1.0, 1.0, 1.0 },
    .translation = { 0, 0, 0 }
};

// Declaration of our global transformation matrices
glm::mat4 proj_matrix;

Engine::Engine(int width, int height)
{
    m_fb = new Framebuffer(width, height);
    m_window = new Window(m_fb, width, height);
    m_light = new Light(glm::vec3(0, 0, 1));
    m_camera = new Camera(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
};

// Setup function to initialize variables and game objects
void Engine::setup()
{
    // Initialize the scene light direction

    // Initialize the perspective projection matrix
    float aspect = (float)m_window->get_width() / (float)m_window->get_height();
    float fov_y = 3.141592 / 3.0; // the same as 180/3, or 60deg
    float fov_x = atan(tan(fov_y / 2) * aspect) * 2;
    float near = 0.1;
    float far = 10.0;

    proj_matrix = glm::perspectiveLH(fov_y, aspect, near, far);

    // Initialize frustum planes with a point and a normal
    init_frustum_planes(fov_x, fov_y, near, far);

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
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            m_is_running = false;
            break;
        case SDL_MOUSEWHEEL:
            if (event.wheel.preciseY > 0) {
                mesh.scale = mesh.scale * (float)1.1;
            } else {
                mesh.scale = mesh.scale / (float)1.1;
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                m_is_running = false;
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
                auto velocity = m_camera->m_direction * sensitivity * m_delta;
                m_camera->m_position = m_camera->m_position + velocity;
                break;
            }
            if (event.key.keysym.sym == SDLK_s) {
                auto velocity = m_camera->m_direction * sensitivity * m_delta;
                m_camera->m_position = m_camera->m_position - velocity;
                break;
            }
            if (event.key.keysym.sym == SDLK_a) {
                auto velocity = glm::vec3(-1.0, 0, 0) * sensitivity * m_delta;
                m_camera->m_position = m_camera->m_position + velocity;
                break;
            }
            if (event.key.keysym.sym == SDLK_d) {
                auto velocity = glm::vec3(1.0, 0, 0) * sensitivity * m_delta;
                m_camera->m_position = m_camera->m_position + velocity;
                break;
            }
            break;
        }
    }
}

// Update function frame by frame with a fixed time step
void Engine::update()
{
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - m_previous);
    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    // Get a delta time factor converted to seconds to be used to update our game objects
    m_delta = (SDL_GetTicks() - m_previous) / 1000.0;
    m_previous = SDL_GetTicks();

    triangles_to_render.clear();

    // Change the mesh scale, rotation, and translation values per animation frame
    mesh.rotation.x -= 0.2 * m_delta;
    mesh.rotation.y -= 0.2 * m_delta;
    mesh.rotation.z += 0.0 * m_delta;
    mesh.translation.z = 5.0;

    auto view_matrix = glm::lookAtLH(m_camera->m_position, glm::vec3(0, 0, 0), m_camera->m_up);

    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
    auto world_matrix = glm::mat4(1.0) * view_matrix;
    world_matrix = glm::scale(world_matrix, mesh.scale);
    world_matrix = glm::translate(world_matrix, mesh.translation);
    world_matrix = glm::rotate(world_matrix, mesh.rotation.x, glm::vec3(1, 0, 0));
    world_matrix = glm::rotate(world_matrix, mesh.rotation.y, glm::vec3(0, 1, 0));
    world_matrix = glm::rotate(world_matrix, mesh.rotation.z, glm::vec3(0, 0, 1));

    // Update camera look at target to create view matrix

    // Loop all triangle faces of our mesh
    for (auto& mesh_face : mesh.faces) {
        auto vector_a = glm::vec3(world_matrix * glm::vec4(mesh_face.a.point, 1.0));
        auto vector_b = glm::vec3(world_matrix * glm::vec4(mesh_face.b.point, 1.0));
        auto vector_c = glm::vec3(world_matrix * glm::vec4(mesh_face.c.point, 1.0));

        // Get the vector subtraction of B-A and C-A
        glm::vec3 vector_ab = glm::normalize(vector_b - vector_a);
        glm::vec3 vector_ac = glm::normalize(vector_c - vector_a);
        glm::vec3 normal = glm::normalize(glm::cross(vector_ab, vector_ac));

        // Backface culling test to see if the current face should be projected
        if (m_fb->should_cull_backface()) {
            // Find the vector between vertex A in the triangle and the camera origin
            glm::vec3 origin = { 0, 0, 0 };
            glm::vec3 camera_ray = origin - vector_a;

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            float dot_normal_camera = glm::dot(normal, camera_ray);

            // Backface culling, bypassing triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        Polygon polygon(vector_a, vector_b, vector_c, mesh_face.a.uv, mesh_face.b.uv, mesh_face.c.uv);
        auto triangles = polygon.clipped_triangles();

        // Loops all the assembled triangles after clipping
        for (size_t t = 0; t < triangles.size(); t++) {
            Triangle triangle_after_clipping = triangles[t];

            glm::vec4 projected_points[3];

            // Loop all three vertices to perform projection and conversion to screen space
            for (int j = 0; j < 3; j++) {
                // Project the current vertex using a perspective projection matrix
                projected_points[j] = proj_matrix * triangle_after_clipping.points[j];

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
            triangles_to_render.push_back(triangle_to_render);
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
    for (auto& triangle : triangles_to_render) {

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
