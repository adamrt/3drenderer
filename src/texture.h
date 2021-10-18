#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct tex2_t {
    float u, v;
} tex2_t;

extern int texture_width;
extern int texture_height;
extern uint32_t* mesh_texture;
extern const uint8_t REDBRICK_TEXTURE[];

#endif
