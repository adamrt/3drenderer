#include "Renderer.h"
#include "vector.h"
#include <utility>
#include <algorithm> // std::swap
#include "texture.h"

Renderer::Renderer()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
    }

    // Set width and height of the SDL window with the max screen resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int fullscreen_width = display_mode.w;
    int fullscreen_height = display_mode.h;

    m_width = fullscreen_width / 2.0;
    m_height = fullscreen_height / 2.0;

    // Create a SDL Window
    window = SDL_CreateWindow(NULL, 0, 0, m_width, m_height, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
    }

    // Create a SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
    }

    // Allocate the required memory in bytes to hold the color buffer and the z-buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * m_width * m_height);
    depth_buffer = (float*)malloc(sizeof(float) * m_width * m_height);

    // Creating a SDL texture that is used to display the color buffer
    colorbuffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        m_width,
        m_height);
}

void Renderer::set_render_method(RenderMethod method)
{
    render_method = method;
}

void Renderer::set_cull_method(CullMethod method)
{
    cull_method = method;
}

bool Renderer:: should_render_wire()
{
    return render_method == RenderMethod::Wire || render_method == RenderMethod::WireVertex || render_method == RenderMethod::FillTriangleWire || render_method == RenderMethod::TexturedWire;
}

bool Renderer:: should_render_wire_vertex()
{
    return (render_method == RenderMethod::WireVertex);
}

bool Renderer:: should_render_filled_triangle()
{
    return (render_method == RenderMethod::FillTriangle || render_method == RenderMethod::FillTriangleWire);
}

bool Renderer:: should_render_textured_triangle()
{
    return (render_method == RenderMethod::Textured || render_method == RenderMethod::TexturedWire);
}

bool Renderer:: should_cull_backface()
{
    return cull_method == CullMethod::Backface;
}

void Renderer::draw_grid()
{
    for (int y = 0; y < m_height; y += 10) {
        for (int x = 0; x < m_width; x += 10) {
            color_buffer[(m_width * y) + x] = 0xFF444444;
        }
    }
}

void Renderer::draw_pixel(int x, int y, uint32_t color)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    color_buffer[(m_width * y) + x] = color;
}

void Renderer::draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)longest_side_length;
    float y_inc = delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void Renderer::draw_rect(int x, int y, int width, int height, uint32_t color)
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void Renderer::render_color_buffer()
{
    SDL_UpdateTexture(
        colorbuffer_texture,
        NULL,
        color_buffer,
        (int)(m_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, colorbuffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Renderer::clear_color_buffer(uint32_t color)
{
    for (int i = 0; i < m_width * m_height; i++) {
        color_buffer[i] = color;
    }
}

void Renderer::clear_z_buffer()
{
    for (int i = 0; i < m_width * m_height; i++) {
        depth_buffer[i] = 1.0;
    }
}

float Renderer:: get_zbuffer_at(int x, int y)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return 1.0;
    }
    return depth_buffer[(m_width * y) + x];
}

void Renderer::update_zbuffer_at(int x, int y, float value)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    depth_buffer[(m_width * y) + x] = value;
}

Renderer::~Renderer()
{
    free(color_buffer);
    free(depth_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/* Return the barycentric weights alpha, beta, and gamma for point p

            A
           /|\
          / | \
         /  |  \
        /  (p)  \
       /  /   \  \
      / /       \ \
     //           \\
    B ------------- C
*/
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    // Find the vectors between the vertices ABC and point p
    vec2_t ab = vec2_sub(b, a);
    vec2_t bc = vec2_sub(c, b);
    vec2_t ac = vec2_sub(c, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t bp = vec2_sub(p, b);

    // Calcualte the area of the full triangle ABC using cross product (area of parallelogram)
    float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);

    // Weight alpha is the area of subtriangle BCP divided by the area of the full triangle ABC
    float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;

    // Weight beta is the area of subtriangle ACP divided by the area of the full triangle ABC
    float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;

    // Weight gamma is easily found since barycentric cooordinates always add up to 1
    float gamma = 1 - alpha - beta;

    vec3_t weights = { alpha, beta, gamma };
    return weights;
}

// Draw a triangle using three raw line calls
void Renderer::draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

// Function to draw a solid pixel at position (x,y) using depth interpolation
void Renderer::draw_triangle_pixel(
    int x, int y, uint32_t color,
    vec4_t point_a, vec4_t point_b, vec4_t point_c)
{
    // Create three vec2 to find the interpolation
    vec2_t p = { (float)x, (float)y };
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Interpolate the value of 1/w for the current pixel
    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
        // Draw a pixel at position (x,y) with a solid color
        draw_pixel(x, y, color);

        // Update the z-buffer value with the 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
}

// Function to draw the textured pixel at position (x,y) using depth interpolation
void Renderer::draw_triangle_texel(
    int x, int y, uint32_t* texture,
    vec4_t point_a, vec4_t point_b, vec4_t point_c,
    tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{
    vec2_t p = { (float)x, (float)y };
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Variables to store the interpolated values of U, V, and also 1/w for the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    // Also interpolate the value of 1/w for the current pixel
    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Now we can divide back both interpolated values by 1/w
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // Map the UV coordinate to the full texture width and height
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
        // Draw a pixel at position (x,y) with the color that comes from the mapped texture
        draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);

        // Update the z-buffer value with the 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
}

/* Draw a textured triangle based on a texture array of colors.
   We split the original triangle in two, half flat-bottom and half flat-top.

          v0
          /\
         /  \
        /    \
       /      \
     v1--------\
       \_       \
          \_     \
             \_   \
                \_ \
                   \\
                     \
                      v2
*/
void Renderer::draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t* texture)
{
    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(z0, z1);
        std::swap(w0, w1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(z1, z2);
        std::swap(w1, w2);
        std::swap(u1, u2);
        std::swap(v1, v2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(z0, z1);
        std::swap(w0, w1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }

    // Flip the V component to account for inverted UV-coordinates (V grows downwards)
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    // Create vector points and texture coords after we sort the vertices
    vec4_t point_a = { (float)x0, (float)y0, z0, w0 };
    vec4_t point_b = { (float)x1, (float)y1, z1, w1 };
    vec4_t point_c = { (float)x2, (float)y2, z2, w2 };
    tex2_t a_uv = { u0, v0 };
    tex2_t b_uv = { u1, v1 };
    tex2_t c_uv = { u2, v2 };

    // Render the upper part of the triangle (flat-bottom)
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0)
        inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                std::swap(x_start, x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }

    // Render the bottom part of the triangle (flat-top)
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0)
        inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                std::swap(x_start, x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }
}

/* Draw a filled triangle with the flat-top/flat-bottom method
   We split the original triangle in two, half flat-bottom and half flat-top

            (x0,y0)
              / \
             /   \
            /     \
           /       \
          /         \
     (x1,y1)---------\
         \_           \
            \_         \
               \_       \
                  \_     \
                     \    \
                       \_  \
                          \_\
                             \
                           (x2,y2)
*/
void Renderer::draw_filled_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color)
{
    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(z0, z1);
        std::swap(w0, w1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(z1, z2);
        std::swap(w1, w2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(z0, z1);
        std::swap(w0, w1);
    }

    // Create three vector points after we sort the vertices
    vec4_t point_a = { (float)x0, (float)y0, z0, w0 };
    vec4_t point_b = { (float)x1, (float)y1, z1, w1 };
    vec4_t point_c = { (float)x2, (float)y2, z2, w2 };

    // Render the upper part of the triangle (flat-bottom)
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0)
        inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                std::swap(x_start, x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with a solid color
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }

    // Render the bottom part of the triangle (flat-top)
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0)
        inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                std::swap(x_start, x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with a solid color
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }
}
