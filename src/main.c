#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

bool is_running = false;
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
uint32_t *color_buffer = NULL;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;

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

void clear_color_buffer(uint32_t color) {
  for (int y = 0; y < SCREEN_HEIGHT; y++){
    for (int x = 0; x < SCREEN_WIDTH; x++){
      color_buffer[(SCREEN_WIDTH * y) + x] = color;
    }
  }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(SCREEN_WIDTH * sizeof(uint32_t)));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_grid(0xFFAAAAAA);
  draw_rect(200, 100, 500, 350, 0xFFC0CB);

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

void destroy_window(void) {
  free(color_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

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
