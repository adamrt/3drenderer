#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "upng.h"
#include <stdint.h>

extern int texture_width;
extern int texture_height;

uint32_t* load_png_texture_data(std::string filename);

#endif
