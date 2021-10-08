#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"


bool is_running = false;

bool setup(void);
void process_input(void);
void update(void);
void render(void);

int main(void){
    if (!initialize_window()) {
        return 1;
    };

    if (!setup()) {
        return 1;
    };

    is_running = true;

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}

bool setup(void) {
    color_buffer = malloc((int)sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
    if (!color_buffer) {
        fprintf(stderr, "Error allocating for color_buffer\n");
        return false;
    }

    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!color_buffer_texture) {
        fprintf(stderr, "Error creating texture\n");
        return false;
    }

    return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type){
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            is_running = false;
        }
        break;
    }
}

void update(void) {

}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_grid(0xFFAAAAAA);
    draw_rect(200, 100, 500, 350, 0xFF00FF);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}
