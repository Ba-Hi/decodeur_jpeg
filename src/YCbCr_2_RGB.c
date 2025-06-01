#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pixel.h"


/*
* Transforme les valeurs YCbCr en RGB
* Entrée: 
*   Blocs: MCUs(complet) compo des blocs Y Cb Cr (ordre donné dans l'entete)
* Sortie:
*   Blocs: compo des blocs R G B (valeurs entre 0 et 255)
*/

Pixel **convert_ycbcr_2_rgb(float **bloc_Y, float **bloc_Cb, float **bloc_Cr, int hauteur, int largeur) {
    
    // Debuuug encore
    if (bloc_Y == NULL) {
        fprintf(stderr, "Erreur Y block NULL convert_ycbcr_2_rgb\n");
        return NULL;
    }
    
    if (bloc_Cb == NULL) {
        fprintf(stderr, "Erreur Cb block NULL convert_ycbcr_2_rgb\n");
        return NULL;
    }
    
    if (bloc_Cr == NULL) {
        fprintf(stderr, "Erreur Cr block NULL convert_ycbcr_2_rgb\n");
        return NULL;
    }
    
    
    /* Initialisation du bloc RGB*/
    Pixel **RGB = (Pixel**)malloc(hauteur * sizeof(Pixel *));
    if (RGB == NULL) {
        fprintf(stderr, "Erreur allocation mémoire\n");
        return NULL;
    }
    
    for (int i = 0; i < hauteur; i++) {
        RGB[i] = (Pixel*)malloc(largeur * sizeof(Pixel));
        if (RGB[i] == NULL) {
            fprintf(stderr, "Erreur allocation mémoire colone %d\n", i);
            // Freeee encore
            for (int j = 0; j < i; j++) {
                free(RGB[j]);
            }
            free(RGB);
            return NULL;
        }
        
        // Initialise tooout pixels en noir
        for (int j = 0; j < largeur; j++) {
            RGB[i][j].r = 0;
            RGB[i][j].g = 0;
            RGB[i][j].b = 0;
        }
    }

    /*Version avec calcul simplifié*/
    for (int i = 0; i < hauteur; i++) {
        /* Pour nous aider à debug*/
        if (bloc_Y[i] == NULL) {
            fprintf(stderr, "Error: Y block row %d is NULL\n", i);
            continue;
        }
        if (bloc_Cb[i] == NULL) {
            fprintf(stderr, "Error: Cb block row %d is NULL\n", i);
            continue;
        }
        if (bloc_Cr[i] == NULL) {
            fprintf(stderr, "Error: Cr block row %d is NULL\n", i);
            continue;
        }
        
        for (int j = 0; j < largeur; j++) {
            
            int r, g, b;
            
            // Version avec calcul simplifié
            if (j < largeur) {
                r = (int)(bloc_Y[i][j] + 1.402 * (bloc_Cr[i][j] - 128));
                g = (int)(bloc_Y[i][j] - 0.34414 * (bloc_Cb[i][j] - 128) - 0.71414 * ((bloc_Cr[i][j] - 128)));
                b = (int)(bloc_Y[i][j] + 1.772 * (bloc_Cb[i][j] - 128));
                
                /* Saturation entre 0 et 255 */
                r = fmin(fmax(r, 0), 255);
                g = fmin(fmax(g, 0), 255);
                b = fmin(fmax(b, 0), 255);

                /* Pixel rgb stockééééé */
                RGB[i][j].r = r;
                RGB[i][j].g = g;
                RGB[i][j].b = b;
            }
        }
    }
    return RGB;
}