#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include "upng.h"

typedef struct tex2_t {
    float u, v;
} tex2_t;

extern int texture_width;
extern int texture_height;

extern upng_t *png_texture;
extern uint32_t *mesh_texture;

void load_png_texture_data(char *filename);

#endif
