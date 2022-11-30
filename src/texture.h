#pragma once

#include "upng.h"
#include <stdint.h>
#include <string>

extern int texture_width;
extern int texture_height;

uint32_t* load_png_texture_data(std::string filename);
