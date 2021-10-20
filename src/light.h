#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"

typedef struct {
    Vec3 direction;
} Light;

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor);

extern Light light;

#endif
