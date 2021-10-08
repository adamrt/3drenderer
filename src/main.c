#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"

#define N_POINTS (9 * 9 * 9)

vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = { 0, 0, -5 };
vec3_t cube_rotation = { 0, 0, 0 };

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

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

    int point_count = 0;
    // load cube points
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                vec3_t new_point = { x, y, z };
                cube_points[point_count] = new_point;
                point_count++;
            }
        }
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


vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };
    return projected_point;
}

void update(void) {
    int ms_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (ms_to_wait > 0) {
        SDL_Delay(ms_to_wait);
    }
    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    for (int i = 0; i < N_POINTS; i++) {
        vec3_t point = cube_points[i];

        // Rotate the point
        vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

        // Move the transformed point away from the camera (minus negative);
        transformed_point.z -= camera_position.z;

        // Project the transformed point
        vec2_t projected_point = project(transformed_point);

        projected_points[i] = projected_point;
    }
}

void render(void) {
    draw_grid(0xFF333333);

    for (int i = 0; i < N_POINTS; i++) {
        vec2_t projected_point = projected_points[i];
        draw_rect(
                  projected_point.x + (SCREEN_WIDTH / 2),
                  projected_point.y + (SCREEN_HEIGHT / 2),
                  2,
                  2,
                  0xFFFFFF00
                  );
    }

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}
