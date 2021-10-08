#include "display.h"

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
            color_buffer[(SCREEN_WIDTH * y) + x] = color;
        }
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int current_x = x + i;
            int current_y = y + j;
            color_buffer[(SCREEN_WIDTH * current_y) + current_x] = color;
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
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
