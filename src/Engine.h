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

    Window* m_window;
    Framebuffer* m_fb;
    Light* m_light;
    Camera* m_camera;
};
