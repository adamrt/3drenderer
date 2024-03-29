#include <algorithm>
#include <cmath>

#include "Framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
{
    m_width = width;
    m_height = height;

    // Initialize render mode and triangle culling method
    set_render_method(RenderMethod::Textured);
    set_cull_method(CullMethod::Backface);

    // Allocate the required memory in bytes to hold the color buffer and the z-buffer
    m_color.resize(m_width * m_height);
    m_depth.resize(m_width * m_height);
}

void Framebuffer::clear_color(uint32_t color)
{
    for (int i = 0; i < m_width * m_height; i++) {
        m_color[i] = color;
    }
}

void Framebuffer::clear_depth()
{
    for (int i = 0; i < m_width * m_height; i++) {
        m_depth[i] = 1.0;
    }
}

float Framebuffer::get_depth(int x, int y)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return 1.0;
    }
    return m_depth[(m_width * y) + x];
}

void Framebuffer::set_depth(int x, int y, float depth)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    m_depth[(m_width * y) + x] = depth;
}

void Framebuffer::draw_grid()
{
    for (int y = 0; y < m_height; y += 10) {
        for (int x = 0; x < m_width; x += 10) {
            m_color[(m_width * y) + x] = 0xFF444444;
        }
    }
}

void Framebuffer::draw_pixel(int x, int y, uint32_t color)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    m_color[(m_width * y) + x] = color;
}

void Framebuffer::draw_line(int x0, int y0, int x1, int y1, uint32_t color)
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

void Framebuffer::draw_rect(int x, int y, int width, int height, uint32_t color)
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

// Draw a triangle using three raw line calls
void Framebuffer::draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

// Function to draw a solid pixel at position (x,y) using depth interpolation
void Framebuffer::draw_triangle_pixel(
    int x, int y, uint32_t color,
    glm::vec4 point_a, glm::vec4 point_b, glm::vec4 point_c)
{
    // Create three vec2 to find the interpolation
    glm::vec2 p = { (float)x, (float)y };
    glm::vec2 a = point_a;
    glm::vec2 b = point_b;
    glm::vec2 c = point_c;

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    glm::vec3 weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Interpolate the value of 1/w for the current pixel
    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    if (interpolated_reciprocal_w < get_depth(x, y)) {
        // Draw a pixel at position (x,y) with a solid color
        draw_pixel(x, y, color);

        // Update the z-buffer value with the 1/w of this current pixel
        set_depth(x, y, interpolated_reciprocal_w);
    }
}

// Function to draw the textured pixel at position (x,y) using depth interpolation
void Framebuffer::draw_triangle_texel(
    int x, int y, uint32_t* texture,
    glm::vec4 point_a, glm::vec4 point_b, glm::vec4 point_c,
    glm::vec2 a_uv, glm::vec2 b_uv, glm::vec2 c_uv)
{
    glm::vec2 p = { (float)x, (float)y };
    glm::vec2 a = point_a;
    glm::vec2 b = point_b;
    glm::vec2 c = point_c;

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    glm::vec3 weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Variables to store the interpolated values of U, V, and also 1/w for the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
    interpolated_u = (a_uv.x / point_a.w) * alpha + (b_uv.x / point_b.w) * beta + (c_uv.x / point_c.w) * gamma;
    interpolated_v = (a_uv.y / point_a.w) * alpha + (b_uv.y / point_b.w) * beta + (c_uv.y / point_c.w) * gamma;

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
    if (interpolated_reciprocal_w < get_depth(x, y)) {
        // Draw a pixel at position (x,y) with the color that comes from the mapped texture
        draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);

        // Update the z-buffer value with the 1/w of this current pixel
        set_depth(x, y, interpolated_reciprocal_w);
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
void Framebuffer::draw_textured_triangle(
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
    glm::vec4 point_a = { (float)x0, (float)y0, z0, w0 };
    glm::vec4 point_b = { (float)x1, (float)y1, z1, w1 };
    glm::vec4 point_c = { (float)x2, (float)y2, z2, w2 };
    glm::vec2 a_uv = { u0, v0 };
    glm::vec2 b_uv = { u1, v1 };
    glm::vec2 c_uv = { u2, v2 };

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
void Framebuffer::draw_filled_triangle(
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
    glm::vec4 point_a = { (float)x0, (float)y0, z0, w0 };
    glm::vec4 point_b = { (float)x1, (float)y1, z1, w1 };
    glm::vec4 point_c = { (float)x2, (float)y2, z2, w2 };

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

void Framebuffer::set_render_method(RenderMethod method)
{
    render_method = method;
}

void Framebuffer::set_cull_method(CullMethod method)
{
    cull_method = method;
}

bool Framebuffer::should_render_wire()
{
    return render_method == RenderMethod::Wire || render_method == RenderMethod::WireVertex || render_method == RenderMethod::FillTriangleWire || render_method == RenderMethod::TexturedWire;
}

bool Framebuffer::should_render_wire_vertex()
{
    return (render_method == RenderMethod::WireVertex);
}

bool Framebuffer::should_render_filled_triangle()
{
    return (render_method == RenderMethod::FillTriangle || render_method == RenderMethod::FillTriangleWire);
}

bool Framebuffer::should_render_textured_triangle()
{
    return (render_method == RenderMethod::Textured || render_method == RenderMethod::TexturedWire);
}

bool Framebuffer::should_cull_backface()
{
    return cull_method == CullMethod::Backface;
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
glm::vec3 barycentric_weights(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p)
{
    // Find the vectors between the vertices ABC and point p
    glm::vec2 ab = b - a;
    glm::vec2 bc = c - b;
    glm::vec2 ac = c - a;
    glm::vec2 ap = p - a;
    glm::vec2 bp = p - b;

    // Calcualte the area of the full triangle ABC using cross product (area of parallelogram)
    float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);

    // Weight alpha is the area of subtriangle BCP divided by the area of the full triangle ABC
    float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;

    // Weight beta is the area of subtriangle ACP divided by the area of the full triangle ABC
    float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;

    // Weight gamma is easily found since barycentric cooordinates always add up to 1
    float gamma = 1 - alpha - beta;

    glm::vec3 weights = { alpha, beta, gamma };
    return weights;
}
