#include "display.h"
#include "triangle.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
float *z_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;
int window_width = 0;
int window_height = 0;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    int fullscreen_width = display_mode.w;
    int fullscreen_height = display_mode.h;

    // This is a hack to force SDL to upscale pixels to our fullscreen resolution.
    // Gives a pixelated look and requires less performance.
    window_width = fullscreen_width / 2.0;
    window_height = fullscreen_height / 2.0;

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, fullscreen_width, fullscreen_height, 0);
    if (!window) {
        fprintf(stderr, "Error creating SDL window\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    return true;
}

// draw_grid modifies the color_buffer to have a dot every 10x10 block.
void draw_grid(uint32_t color) {
    for (int y = 0; y < window_height; y += 10){
        for (int x = 0; x < window_width; x += 10){
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

void draw_points(int x0, int y0, int x1, int y1, int x2, int y2, int width, int height, uint32_t color) {
    draw_rect(x0, y0, width, height, color);
    draw_rect(x1, y1, width, height, color);
    draw_rect(x2, y2, width, height, color);
}


void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = color;
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
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++){
        for (int x = 0; x < window_width; x++){
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void clear_z_buffer() {
    for (int y = 0; y < window_height; y++){
        for (int x = 0; x < window_width; x++){
            z_buffer[(window_width * y) + x] = 1.0;
        }
    }
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
