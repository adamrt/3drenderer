#pragma once
#include <SDL2/SDL.h>
#include <cstdint>

#include "Framebuffer.h"

constexpr int FPS = 30;
constexpr int FRAME_TARGET_TIME = (1000 / FPS);

class Window {
public:
    Window(Framebuffer* fb, int width, int height);
    ~Window();

    void render();

    void set_title(std::string title);

    int get_width() { return m_width; }
    int get_height() { return m_height; }

private:
    Framebuffer* m_fb;
    int m_width;
    int m_height;

    SDL_Window* m_window = NULL;
    SDL_Renderer* m_renderer = NULL;
    SDL_Texture* m_texture = NULL;
};
