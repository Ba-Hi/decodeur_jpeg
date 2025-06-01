#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "traiter_image_noir_blanc.h"
#include "bitstream.h"
#include "quantification_inverse.h"
#include "zz_inverse.h"
#include "idct.h"
#include "idct_rapide.h"
#include "YCbCr_2_RGB_nb.h"
#include "ecrire_ppm.h"
#include "extraire_bloc.h"
#include "up_sampling.h"
#include "reassembler_mcus.h"

void liberer_bloc_triple_int16(uint8_t ***bloc) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            free(bloc[i][j]);
        }
        free(bloc[i]);
    }
    free(bloc);
}

void traiter_image_noir_blanc(metadonnees_jpeg_t *meta, char *nom_image, uint8_t use_loeffler){

    /*Calcul du nombre de blocs de Y*/
    uint32_t nb_blocs_horizontal_Y = (meta->largeur + 7) / 8;
    uint32_t nb_blocs_vertical_Y   = (meta->hauteur + 7) / 8;
    uint32_t nb_blocs_Y            = nb_blocs_horizontal_Y * nb_blocs_vertical_Y;

    /*On extrait les blocs Y*/
    float **blocs_Y = extraire_blocs_y_noir_et_blanc(meta->stream, meta);
    if (!blocs_Y) {
        fprintf(stderr, "Erreur lors de l'extraction des blocs Y.\n");
        return;
    }

    /*Maintenant qu'on a les blocs_Y extraits, on va appliquer sur chacun :
    -Quantification inverse
    -Zigzag inverse
    -iDCT*/

    /*Allocation de l'image RGB*/
    typedef Pixel BlocRGB[8][8];
    Pixel (*image_rgb)[8][8] = malloc(sizeof(BlocRGB) * nb_blocs_Y);
    if (!image_rgb) {
        fprintf(stderr, "Erreur allocation image_rgb\n");
        return;
    }

    for (uint32_t bloc = 0; bloc < nb_blocs_Y; bloc++) {
        /*Quantification Inverse de chaque élément dans blocs_y*/
        quantification_inverse(blocs_Y[bloc], meta->tables_quantif[0]);

        /*zz inverse de chaque element dans blocs_y*/
        float bloc_dequant[8][8];
        zigzagInverse(blocs_Y[bloc], bloc_dequant);

        
        /*iDCT*/
        float bloc_Y_idct[8][8];
        if (use_loeffler) {
            int16_t ***int_bloc_dequant = malloc(8 * sizeof(int16_t **));
            uint8_t ***uint_bloc_result = malloc(8 * sizeof(uint8_t **));
            for (int i = 0; i < 8; i++) {
                int_bloc_dequant[i] = malloc(8 * sizeof(int16_t *));
                uint_bloc_result[i] = malloc(8 * sizeof(uint8_t *));
                for (int j = 0; j < 8; j++) {
                    int_bloc_dequant[i][j] = malloc(sizeof(int16_t));
                    uint_bloc_result[i][j] = malloc(sizeof(uint8_t));
                    *int_bloc_dequant[i][j] = (int16_t)bloc_dequant[i][j];
                }
            }
            idct_loeffler_2d(int_bloc_dequant, uint_bloc_result);
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    bloc_Y_idct[i][j] = (float)*uint_bloc_result[i][j];
                }
            }

            liberer_bloc_triple_int16(uint_bloc_result);
            liberer_bloc_triple_int16((uint8_t ***) int_bloc_dequant);
            
        } else {
            idct_8x8(bloc_dequant, bloc_Y_idct);
        }


        float **bloc_Y_ptr = malloc(8 * sizeof(float*));
            for (int i = 0; i < 8; i++) bloc_Y_ptr[i] = bloc_Y_idct[i];  // pointeurs sur les lignes

        /*Conversion et affichage*/
        Pixel **rgb = convert_ycbcr_2_rgb_noir_et_blanc(bloc_Y_ptr, 8);

        for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    /*pixel j,k dans le bloc i, remplir avec rgb*/
                    image_rgb[bloc][j][k] = rgb[j][k]; 
                }
            }


        free(bloc_Y_ptr);
        for (int l = 0; l < 8; l++) {
            free(rgb[l]);
        }
        free(rgb);
        
    }

    /*Concaténation des blocs 8x8 et image finale*/
    Pixel **image_finale = malloc(sizeof(Pixel*) * meta->hauteur);
    if (!image_finale) {
    fprintf(stderr, "Erreur allocation image_finale\n");
    free(image_rgb);
    return;
    }
    for (int i = 0; i < meta->hauteur; i++) {
        image_finale[i] = malloc(sizeof(Pixel) * meta->largeur);
        if (!image_finale[i]) {
            fprintf(stderr, "Erreur allocation ligne image_finale[%d]\n", i);
            // libération partielle ici si tu veux être ultra rigoureuse
            return;
        }
    }

    int blocs_par_ligne = nb_blocs_horizontal_Y;

    for (uint32_t bloc_y = 0; bloc_y < nb_blocs_vertical_Y; bloc_y++) {
        for (uint32_t bloc_x = 0; bloc_x < nb_blocs_horizontal_Y; bloc_x++) {
            int bloc_index = bloc_y * blocs_par_ligne + bloc_x;

            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    int y = bloc_y * 8 + i;
                    int x = bloc_x * 8 + j;
                    if (y < meta->hauteur && x < meta->largeur) {
                        image_finale[y][x] = image_rgb[bloc_index][i][j];
                    }
                }
            }
        }
    }

    /*Création du nom du fichier PGM*/
    char nom_pgm[256];
    char *point = strrchr(nom_image, '.');
    if (point) {
        size_t len = point - nom_image;
        strncpy(nom_pgm, nom_image, len);
        nom_pgm[len] = '\0';
    } else {
        strcpy(nom_pgm, nom_image);
    }
    strcat(nom_pgm, ".pgm");

    ecrire_image_pgm(nom_pgm, image_finale, meta->largeur, meta->hauteur);

    for (uint32_t i = 0; i < nb_blocs_Y; i++){
        free(blocs_Y[i]);
    }
    free(blocs_Y);
    for (int y = 0; y < meta->hauteur; y++){
        free(image_finale[y]);
    }
    free(image_finale);
    free(image_rgb);
}
