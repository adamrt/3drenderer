#pragma once
#include <SDL2/SDL.h>
#include <cstdint>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

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

    bool m_show_demo_window = true;
    bool m_show_another_window = false;
    ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO* m_io;
};
