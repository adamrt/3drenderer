#pragma once
#include <SDL2/SDL.h>
#include <cstdint>

#include "Framebuffer.h"
#include "texture.h"
#include "vector.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

class Window {
public:
    Window();
    ~Window();

    void set_title(std::string title);
    void update(Framebuffer* framebuffer);

    int get_width() { return m_width; }
    int get_height() { return m_height; }

private:
    int m_width;
    int m_height;

    SDL_Window* m_window = NULL;
    SDL_Renderer* m_renderer = NULL;
    SDL_Texture* m_texture = NULL;
};
