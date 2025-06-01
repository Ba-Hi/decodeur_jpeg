#ifndef TRAITER_IMAGE_NOIR_BLANC_H
#define TRAITER_IMAGE_NOIR_BLANC_H

#include <stdint.h>
#include "extraire_bitstream.h"

void traiter_image_noir_blanc(metadonnees_jpeg_t *meta, char *nom_image, uint8_t use_loeffler);

#endif // TRAITER_COULEUR_H