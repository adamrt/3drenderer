#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "upng.h"

int texture_width = 64;
int texture_height = 64;

uint32_t* load_png_texture_data(std::string filename)
{
    upng_t* png_texture = upng_new_from_file(filename.c_str());
    uint32_t* mesh_texture = NULL;

    if (png_texture != NULL) {
        upng_decode(png_texture);
        if (upng_get_error(png_texture) == UPNG_EOK) {
            mesh_texture = (uint32_t*)upng_get_buffer(png_texture);
            texture_width = upng_get_width(png_texture);
            texture_height = upng_get_height(png_texture);
        }
    }
    free(png_texture);
    return mesh_texture;
}

tex2_t tex2_clone(tex2_t* t)
{
    tex2_t result = { t->u, t->v };
    return result;
}
