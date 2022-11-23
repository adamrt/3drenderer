#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <stdint.h>

class Light {
public:
    Light(glm::vec3 direction)
        : direction(direction) {};
    glm::vec3 direction;

    uint32_t calculate_light_color(uint32_t original_color, glm::vec3 normal);

private:
    float intensity_factor(glm::vec3 normal);
};

#endif
