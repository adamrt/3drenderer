#ifndef OPTIONS_H
#define OPTIONS_H

#include "stdbool.h"

typedef struct options_t {
    bool enable_backface_culling;
    bool enable_wireframe;
    bool enable_fill_triangles;
} options_t;

options_t new_options(void) {
    options_t opt = {
        .enable_backface_culling = true,
        .enable_wireframe = true,
        .enable_fill_triangles = true,
    };
    return opt;
}

#endif
