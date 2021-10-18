#include "display.h"
#include "triangle.h"
#include "swap.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;
uint32_t *color_buffer = NULL;
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL\n");
        return false;
    }

    /* SDL_DisplayMode display_mode; */
    /* SDL_GetCurrentDisplayMode(0, &display_mode); */

    /* SCREEN_WIDTH = display_mode.w; */
    /* SCREEN_HEIGHT = display_mode.h; */

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
    if (!window) {
        fprintf(stderr, "Error creating SDL window\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer\n");
        return false;
    }

    // Wayland is an issue I think.
    // SDL_SetWindowFullscreen(0, SDL_WINDOW_FULLSCREEN);

    return true;
}

// draw_grid modifies the color_buffer to have a dot every 10x10 block.
void draw_grid(uint32_t color) {
    for (int y = 0; y < SCREEN_HEIGHT; y += 10){
        for (int x = 0; x < SCREEN_WIDTH; x += 10){
            draw_pixel(x, y, color);
        }
    }
}

// draw_line uses the DDA algorithm to draw a line
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    // Find x and y increments per step
    float x_inc = delta_x / (float)side_length;
    float y_inc = delta_y / (float)side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float inv_slope_1 = (x1 - x0) / (float)(y1 - y0);
    float inv_slope_2 = (x2 - x0) / (float)(y2 - y0);

    float x_start = x0;
    float x_end = x0;

    for (int y = y0; y <= y2; y++) {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float inv_slope_1 = (x2 - x0) / (float)(y2 - y0);
    float inv_slope_2 = (x2 - x1) / (float)(y2 - y1);

    float x_start = x2;
    float x_end = x2;

    for (int y = y2; y >= y0; y--) {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}

// draw_filled_triangle uses the flat-top/flat-bottom method.
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // Sort vertices by y-coord (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }

    // Do again in case y1 > y2 swapped them back
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    // Calculate new vertex (Mx, My) using triangle similariy

    if (y1 == y2) {
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    } else {
        int my = y1;
        int mx = ((float)((x2 - x0) * (y1 - y0)) / (float) (y2 - y0)) + x0;
        fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
        fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
    }



}

void draw_points(int x0, int y0, int x1, int y1, int x2, int y2, int width, int height, uint32_t color) {
    draw_rect(x0, y0, width, height, color);
    draw_rect(x1, y1, width, height, color);
    draw_rect(x2, y2, width, height, color);
}


void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        color_buffer[(SCREEN_WIDTH * y) + x] = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void render_color_buffer(void) {
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(SCREEN_WIDTH * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < SCREEN_HEIGHT; y++){
        for (int x = 0; x < SCREEN_WIDTH; x++){
            color_buffer[(SCREEN_WIDTH * y) + x] = color;
        }
    }
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
