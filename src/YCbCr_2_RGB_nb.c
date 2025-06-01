#include <stdio.h>
#include <stdlib.h>
#include "YCbCr_2_RGB_nb.h"

/*Ici, on convertira uniquement la composante Y en RGB. On ne prend pas en compte les composantes Cb et Cr*/
Pixel **convert_ycbcr_2_rgb_noir_et_blanc(float **bloc_Y, int hauteur){
    int largeur = hauteur;

    if (!bloc_Y){
        fprintf(stderr,"Erreur : bloc vide\n");
        return NULL;
    }

    Pixel **RGB = malloc(hauteur*sizeof(Pixel *));
    if (!RGB){
        fprintf(stderr,"Erreur : allocation impossible\n");
        return NULL;
    }

    for (int i = 0; i < hauteur; i++){
        RGB[i] = malloc(largeur*sizeof(Pixel));
        for (int j = 0; j < largeur; j++){

            uint8_t y = (uint8_t)(bloc_Y[i][j]);
            RGB[i][j].r = y;
            RGB[i][j].g = y;
            RGB[i][j].b = y;
        }
    }
    return RGB;
}