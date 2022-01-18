#ifndef TEXTURE_H
#define TEXTURE_H

#include "upng.h"
#include <stdint.h>

typedef struct {
    float u;
    float v;
} tex2_t;

extern int texture_width;
extern int texture_height;

uint32_t* load_png_texture_data(char* filename);
tex2_t tex2_clone(tex2_t* t);

#endif
