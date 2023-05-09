#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

#include <SDL.h>
#include <SDL2/SDL_video.h>

#include "Window.h"

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#    error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

Window::Window(Framebuffer* fb, int width, int height)
    : m_fb(fb)
    , m_width(width)
    , m_height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        exit(EXIT_FAILURE);
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create a SDL Window
    m_window = SDL_CreateWindow(NULL, 0, 0, m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!m_window) {
        fprintf(stderr, "Error creating SDL window.\n");
        exit(EXIT_FAILURE);
    }

    // Create a SDL renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        exit(EXIT_FAILURE);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();
    (void)m_io;
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer_Init(m_renderer);

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
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::render()
{

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (m_show_demo_window)
        ImGui::ShowDemoWindow(&m_show_demo_window);

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");            // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &m_show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &m_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);              // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&m_clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_io->Framerate, m_io->Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (m_show_another_window) {
        ImGui::Begin("Another Window", &m_show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            m_show_another_window = false;
        ImGui::End();
    }


    ImGui::Render();

    auto row_width = m_width * sizeof(uint32_t);
    auto data = m_fb->get_color_buffer();
    SDL_UpdateTexture(m_texture, NULL, &data[0], row_width);
    
    SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    
    SDL_RenderPresent(m_renderer);
}

void Window::set_title(std::string title)
{
    SDL_SetWindowTitle(m_window, title.c_str());
}
