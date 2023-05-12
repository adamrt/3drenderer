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

    while (engine.is_running()) {
        engine.process_input();
        engine.update();
        engine.render();
    }

    return 0;
}
