#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>

#include <glm/vec3.hpp>

typedef struct {
    glm::vec3 direction;
} light_t;

void init_light(glm::vec3 direction);
glm::vec3 get_light_direction(void);
uint32_t apply_light_intensity(uint32_t original_color, float percentage_factor);

#endif
