#ifndef TRAITER_COULEUR_H
#define TRAITER_COULEUR_H

#include <stdint.h>
#include "extraire_bitstream.h"

void traiter_image_couleur(metadonnees_jpeg_t *meta, char *nom_image, uint8_t use_loeffler);

#endif // TRAITER_COULEUR_H