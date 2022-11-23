#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Light.h"
#include "Window.h"

struct Engine {
    Engine(int width, int height);
    void setup();
    void process_input();
    void update();
    void render();

    bool is_running = true;
    int fps = 0;
    int fps_timer = 0;

    Window* m_window;
    Framebuffer* m_fb;
    Light* m_light;
    Camera* m_camera;
};
