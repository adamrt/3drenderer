#include <SDL2/SDL_video.h>
#include <algorithm>

#include "Framebuffer.h"
#include "Window.h"
#include "texture.h"
#include "vector.h"

Window::Window()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
    }

    // Set width and height of the SDL window with the max screen resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int fullscreen_width = display_mode.w;
    int fullscreen_height = display_mode.h;

    m_width = fullscreen_width / 2.0;
    m_height = fullscreen_height / 2.0;

    // Create a SDL Window
    m_window = SDL_CreateWindow(NULL, 0, 0, m_width, m_height, SDL_WINDOW_SHOWN);
    if (!m_window) {
        fprintf(stderr, "Error creating SDL window.\n");
    }

    // Create a SDL renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if (!m_renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
    }

    // Creating a SDL texture that is used to display the color buffer
    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        m_width,
        m_height);
}

void Window::set_title(std::string title) {
    SDL_SetWindowTitle(m_window, title.c_str());
}

void Window::update(Framebuffer* framebuffer)
{
    SDL_UpdateTexture(
        m_texture,
        NULL,
        &framebuffer->get_color_buffer()[0],
        (int)(m_width * sizeof(uint32_t)));
    SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}

Window::~Window()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
