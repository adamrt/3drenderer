#ifndef OPTIONS_H
#define OPTIONS_H

#include "stdbool.h"

typedef struct options_t {
    bool enable_backface_culling;
    bool enable_wireframe;
    bool enable_fill_triangles;
    bool enable_textured_triangles;
} options_t;

options_t options_default(void);

#endif
