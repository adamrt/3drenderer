#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Light.h"
#include "Window.h"

class Engine {
public:
    Engine(int width, int height);
    void setup();
    void process_input();
    void update();
    void render();

    bool is_running() { return m_is_running; };

private:
    bool m_is_running = true;
    int m_fps = 0;
    int m_fps_timer = 0;
    float m_delta = 0;
    int m_previous = 0;

    Window* m_window;
    Framebuffer* m_fb;
    Light* m_light;
    Camera* m_camera;
};
