#include <SDL2/SDL_video.h>

#include "Window.h"

Window::Window(Framebuffer* fb, int width, int height)
    : m_fb(fb)
    , m_width(width)
    , m_height(height)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        exit(EXIT_FAILURE);
    }

    // Create a SDL Window
    m_window = SDL_CreateWindow(NULL, 0, 0, m_width, m_height, SDL_WINDOW_SHOWN);
    if (!m_window) {
        fprintf(stderr, "Error creating SDL window.\n");
        exit(EXIT_FAILURE);
    }

    // Create a SDL renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if (!m_renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        exit(EXIT_FAILURE);
    }

    // Creating a SDL texture that is used to display the color buffer
    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        m_width,
        m_height);
}

Window::~Window()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::render()
{
    auto row_width = m_width * sizeof(uint32_t);
    auto data = m_fb->get_color_buffer();

    SDL_UpdateTexture(m_texture, NULL, &data[0], row_width);
    SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}

void Window::set_title(std::string title)
{
    SDL_SetWindowTitle(m_window, title.c_str());
}
