#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string>

#include <SDL2/SDL.h>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Engine.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"

int main()
{
    Engine engine(1024, 768);
    engine.setup();
    int fps = 0;
    int fps_timer = 0;

    while (engine.is_running) {
        int now = SDL_GetTicks();
        // count fps in 1 sec (1000 ms)
        if (now > fps_timer + 1000) {
            std::string title = std::to_string(fps);
            title.append(" FPS");
            engine.m_window->set_title(title);
            fps_timer = now;
            fps = 0;
        }

        engine.process_input();
        engine.update();
        engine.render();

        fps++;
    }

    return 0;
}
