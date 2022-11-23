#pragma once
#include <cstdint>
#include <vector>

#include "texture.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum class CullMethod {
    None,
    Backface
};

enum class RenderMethod {
    Wire,
    WireVertex,
    FillTriangle,
    FillTriangleWire,
    Textured,
    TexturedWire
};

glm::vec3 barycentric_weights(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p);

class Framebuffer {
public:
    Framebuffer(int width, int height);

    std::vector<uint32_t> get_color_buffer() { return m_color; };

    void render(void);

    void clear_color(uint32_t color);
    void clear_depth();

    float get_depth(int x, int y);
    void set_depth(int x, int y, float depth);

    void draw_grid(void);
    void draw_pixel(int x, int y, uint32_t color);
    void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
    void draw_rect(int x, int y, int width, int height, uint32_t color);

    void draw_triangle_pixel(int x, int y, uint32_t color, glm::vec4 point_a, glm::vec4 point_b, glm::vec4 point_c);
    void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    void draw_filled_triangle(int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color);
    void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0, float v0, int x1, int y1, float z1, float w1, float u1, float v1, int x2, int y2, float z2, float w2, float u2, float v2, uint32_t* texture);
    void draw_triangle_texel(int x, int y, uint32_t* texture, glm::vec4 point_a, glm::vec4 point_b, glm::vec4 point_c, glm::vec2 a_uv, glm::vec2 b_uv, glm::vec2 c_uv);

    RenderMethod render_method = RenderMethod::Textured;
    CullMethod cull_method = CullMethod::Backface;
    void set_render_method(RenderMethod method);
    void set_cull_method(CullMethod method);

    bool should_render_wire(void);
    bool should_render_wire_vertex(void);
    bool should_render_textured_triangle(void);
    bool should_render_filled_triangle(void);
    bool should_cull_backface(void);

private:
    int m_height;
    int m_width;
    std::vector<uint32_t> m_color;
    std::vector<float> m_depth;
};
