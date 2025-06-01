#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "traiter_image_couleur.h"
#include "bitstream.h"
#include "quantification_inverse.h"
#include "zz_inverse.h"
#include "idct.h"
#include "idct_rapide.h"
#include "YCbCr_2_RGB.h"
#include "ecrire_ppm.h"
#include "extraire_bloc.h"
#include "up_sampling.h"
#include "reassembler_mcus.h"

void liberer_bloc(float **blocs, int nb_blocs) {
    if (blocs) {
        for (int i = 0; i < nb_blocs; i++) {
            free(blocs[i]);
        }
        free(blocs);
    }
}


void liberer_bloc_flottant(float **bloc, int v) {
    for (int i = 0; i < v; i++) {
        free(bloc[i]);
    }
    free(bloc);
}

void liberer_mcus(MCU_t *mcus, int nb_mcus, int v_Y, int v_Cb, int v_Cr, int h_Y, int h_Cb, int h_Cr) {
    if (mcus) {
        for (int i = 0; i < nb_mcus; i++) {
            liberer_bloc(mcus[i].blocs_Y, v_Y * h_Y);
            liberer_bloc(mcus[i].blocs_Cb, v_Cb * h_Cb);
            liberer_bloc(mcus[i].blocs_Cr, v_Cr * h_Cr);
        }
        free(mcus);
    }
}

// DRY: mutualise le traitement d'un composant (Y, Cb ou Cr)
void traiter_composant(float **blocks, float **output, int h, int v, uint16_t *table_quantif, uint8_t use_loffler) {
    for (int by = 0; by < v; by++) {
        for (int bx = 0; bx < h; bx++) {
            float bloc_dequant[8][8];
            float *bloc = blocks[bx + by * h];

            quantification_inverse(bloc, table_quantif);
            zigzagInverse(bloc, bloc_dequant);

            // Allocate memoire pour int16_t*** and uint8_t***
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

            if (use_loffler) {
                idct_loeffler_2d(int_bloc_dequant, uint_bloc_result);
            } else {
                float bloc_result[8][8];
                idct_8x8(bloc_dequant, bloc_result);
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        output[by * 8 + i][bx * 8 + j] = bloc_result[i][j];
                    }
                }
            }

            if (use_loffler) {
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        output[by * 8 + i][bx * 8 + j] = (float)*uint_bloc_result[i][j];
                        free(int_bloc_dequant[i][j]);
                        free(uint_bloc_result[i][j]);
                    }
                    free(int_bloc_dequant[i]);
                    free(uint_bloc_result[i]);
                }
                free(int_bloc_dequant);
                free(uint_bloc_result);
            }
        }
    }
}



void traiter_image_couleur(metadonnees_jpeg_t *meta, char *nom_image, uint8_t use_loffler) {
    int h_Y = meta->echantill_horizontal_Y, v_Y = meta->echantill_vertical_Y;
    int h_Cb = meta->echantill_horizontal_Cb, v_Cb = meta->echantill_vertical_Cb;
    int h_Cr = meta->echantill_horizontal_Cr, v_Cr = meta->echantill_vertical_Cr;

    int nb_mcu_x = (meta->largeur + (8 * h_Y - 1)) / (8 * h_Y);
    int nb_mcu_y = (meta->hauteur + (8 * v_Y - 1)) / (8 * v_Y);
    int nb_mcus = nb_mcu_x * nb_mcu_y;

    MCU_t *mcus = decoupe_par_mcu(nb_mcus, meta->stream, meta);

    int hauteur_image = nb_mcu_y * v_Y * 8;
    int largeur_image = nb_mcu_x * h_Y * 8;

    Pixel **image_finale = malloc(sizeof(Pixel*) * hauteur_image);
    for (int i = 0; i < hauteur_image; i++) {
        image_finale[i] = malloc(sizeof(Pixel) * largeur_image);
    }

    for (int my = 0; my < nb_mcu_y; my++) {
        for (int mx = 0; mx < nb_mcu_x; mx++) {
            int mcu_index = mx + my * nb_mcu_x;
            MCU_t mcu = mcus[mcu_index];

            float **bloc_Y = reconstituer_bloc_complet(mcu.blocs_Y, h_Y, v_Y);
            float **bloc_Cb = reconstituer_bloc_complet(mcu.blocs_Cb, h_Cb, v_Cb);
            float **bloc_Cr = reconstituer_bloc_complet(mcu.blocs_Cr, h_Cr, v_Cr);

            traiter_composant(mcu.blocs_Y, bloc_Y, h_Y, v_Y, meta->tables_quantif[0], use_loffler);
            traiter_composant(mcu.blocs_Cb, bloc_Cb, h_Cb, v_Cb, meta->tables_quantif[1], use_loffler);
            traiter_composant(mcu.blocs_Cr, bloc_Cr, h_Cr, v_Cr, meta->tables_quantif[1], use_loffler);

            float **up_Cb = up_C(bloc_Cb, h_Y, v_Y, h_Cb, v_Cb);
            float **up_Cr = up_C(bloc_Cr, h_Y, v_Y, h_Cr, v_Cr);

            if (!up_Cb || !up_Cr) {
                fprintf(stderr, "Error: Upsampling failed for MCU at (%d, %d)\n", mx, my);
                liberer_bloc(up_Cb, v_Y * 8);
                liberer_bloc(up_Cr, v_Y * 8);
                liberer_bloc(bloc_Y, h_Y * 8);
                liberer_bloc(bloc_Cb, h_Cb * 8);
                liberer_bloc(bloc_Cr, h_Cr * 8);
                continue;
            }

            Pixel **pixels = convert_ycbcr_2_rgb(bloc_Y, up_Cb, up_Cr, v_Y * 8, h_Y * 8);

            if (pixels) {
                for (int i = 0; i < v_Y * 8; i++) {
                    for (int j = 0; j < h_Y * 8; j++) {
                        int global_y = my * v_Y * 8 + i;
                        int global_x = mx * h_Y * 8 + j;
                        if (global_y < meta->hauteur && global_x < meta->largeur) {
                            image_finale[global_y][global_x] = pixels[i][j];
                        }
                    }
                    free(pixels[i]);
                }
                free(pixels);
            } else {
                fprintf(stderr, "Error: Failed to convert YCbCr to RGB for MCU at (%d, %d)\n", mx, my);
            }

            liberer_bloc(up_Cb, v_Y * 8);
            liberer_bloc(up_Cr, v_Y * 8);
            liberer_bloc_flottant(bloc_Y, v_Y * 8);
            liberer_bloc(bloc_Cb, h_Cb * 8);
            liberer_bloc(bloc_Cr, h_Cr * 8);
        }
    }

    char nom_ppm[256];
    char *point = strrchr(nom_image, '.');
    if (point) {
        size_t len = point - nom_image;
        strncpy(nom_ppm, nom_image, len);
        nom_ppm[len] = '\0';
    } else {
        strcpy(nom_ppm, nom_image);
    }
    strcat(nom_ppm, ".ppm");

    ecrire_image_ppm(nom_ppm, image_finale, meta->largeur, meta->hauteur);

    for (int i = 0; i < hauteur_image; i++) {
        free(image_finale[i]);
    }
    free(image_finale);
    liberer_mcus(mcus, nb_mcus, v_Y, v_Cb, v_Cr, h_Y, h_Cb, h_Cr);
}