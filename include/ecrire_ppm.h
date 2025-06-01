#ifndef ECRIRE_PPM_H
#define ECRIRE_PPM_H
#include "pixel.h"

void ecrire_image_ppm(const char *nom_fichier, Pixel **image, int largeur, int hauteur);
void ecrire_image_pgm(const char *nom_fichier, Pixel **image, int largeur, int hauteur);

#endif // ECRIRE_PPM_H