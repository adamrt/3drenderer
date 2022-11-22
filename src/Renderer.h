#pragma once
#include "texture.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <cstdint>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

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

class Renderer {
public:
    Renderer();
    ~Renderer();

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    uint32_t* color_buffer = NULL;
    float* depth_buffer = NULL;

    SDL_Texture* colorbuffer_texture = NULL;

    RenderMethod render_method = RenderMethod::Textured;
    CullMethod cull_method = CullMethod::Backface;

    int get_window_width() { return m_width; }
    int get_window_height() { return m_height; }

    void draw_grid(void);
    void draw_pixel(int x, int y, uint32_t color);
    void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
    void draw_rect(int x, int y, int width, int height, uint32_t color);

    void draw_triangle_pixel(int x, int y, uint32_t color, vec4_t point_a, vec4_t point_b, vec4_t point_c);
    void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    void draw_filled_triangle(int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color);
    void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0, float v0, int x1, int y1, float z1, float w1, float u1, float v1, int x2, int y2, float z2, float w2, float u2, float v2, uint32_t* texture);
    void draw_triangle_texel(int x, int y, uint32_t* texture, vec4_t point_a, vec4_t point_b, vec4_t point_c, tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);

    void clear_color_buffer(uint32_t color);
    void clear_z_buffer(void);
    void render_color_buffer(void);

    float get_zbuffer_at(int x, int y);
    void update_zbuffer_at(int x, int y, float value);

    void set_render_method(RenderMethod method);
    void set_cull_method(CullMethod method);
    bool should_render_wire(void);
    bool should_render_wire_vertex(void);
    bool should_render_textured_triangle(void);
    bool should_render_filled_triangle(void);
    bool should_cull_backface(void);

private:
    int m_width;
    int m_height;
};
