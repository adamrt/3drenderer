#include <stdint.h>
#include <stdio.h>

#include "texture.h"

upng_t *png_texture = NULL;
uint32_t *mesh_texture = NULL;

int texture_width = 0;
int texture_height = 0;

void load_png_texture_data(char *filename) {
    png_texture = upng_new_from_file(filename);
    if (png_texture != NULL) {
        upng_decode(png_texture);
        if (upng_get_error(png_texture) == UPNG_EOK) {
            mesh_texture = (uint32_t*)upng_get_buffer(png_texture);
            texture_width = upng_get_width(png_texture);
            texture_height = upng_get_height(png_texture);
        }
    }
}
