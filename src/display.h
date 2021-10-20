#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "triangle.h"

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *color_buffer;
extern float *z_buffer;
extern SDL_Texture *color_buffer_texture;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void int_swap(int *a, int *b);

bool initialize_window(void);

void clear_z_buffer();
void clear_color_buffer(uint32_t color);
void render_color_buffer(void);

void draw_grid(uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_points(int x0, int y0, int x1, int y1, int x2, int y2, int width, int height, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_texel(int x, int y,
                uint32_t *texture,
                Vec4 point_a, Vec4 point_b, Vec4 point_c,
                Tex2 a_uv, Tex2 b_uv, Tex2 c_uv);
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
Vec3 barycentric_weight(Vec2 a, Vec2 b, Vec2 c, Vec2 p);
void destroy_window(void);
#endif
