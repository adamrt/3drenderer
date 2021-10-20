#ifndef OPTIONS_H
#define OPTIONS_H

#include "stdbool.h"

typedef struct {
    bool enable_backface_culling;
    bool enable_wireframe;
    bool enable_fill_triangles;
    bool enable_textured_triangles;
} Options;

#endif
