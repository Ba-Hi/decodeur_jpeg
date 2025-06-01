#ifndef YCBCR2RGB_H
#define YCBCR2RGB_H

#include <stdint.h>
#include "pixel.h"

Pixel **convert_ycbcr_2_rgb(float **bloc_Y, float **bloc_Cb, float **bloc_Cr, int hauteur, int largeur);

#endif // YCBCR2RGB_H
