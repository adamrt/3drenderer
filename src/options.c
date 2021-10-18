#include "options.h"

options_t options_default(void) {
    options_t opt = {
        .enable_backface_culling = true,
        .enable_wireframe = false,
        .enable_fill_triangles = true,
        .enable_textured_triangles = false,
    };
    return opt;
}
