#ifndef OPTIONS_H
#define OPTIONS_H

#include "stdbool.h"

typedef struct Options {
    bool enable_backface_culling;
    bool enable_wireframe;
    bool enable_fill_triangles;
} Options;

Options new_options(void) {
    Options opt = {
        .enable_backface_culling = true,
        .enable_wireframe = true,
        .enable_fill_triangles = true,
    };
    return opt;
}

#endif
