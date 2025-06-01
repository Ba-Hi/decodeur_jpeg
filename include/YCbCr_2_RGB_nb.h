#ifndef CONVERT_NB_H
#define CONVERT_NB_H

#include <stdint.h>
#include <stdio.h>
#include "pixel.h"

Pixel **convert_ycbcr_2_rgb_noir_et_blanc(float **bloc_Y, int hauteur);

#endif /*CONVERT_NB_H*/