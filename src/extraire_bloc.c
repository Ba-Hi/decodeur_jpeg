#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


#include "../include/extraire_bitstream.h"
#include "../include/huffman_decode.h"
#include "../include/extraire_bloc.h"

/*
 * Lire le flux de donnees JPEG (apres SOS) en gérant :
 *  FF 00 -> byte stuffing
 *  FF D0..D7 -> restart markers (ignorés ici)
 *  FF D9 -> EOI (c'est finiii)
 */
uint8_t* lire_flux_donnees(FILE *image, long position, size_t *taille_flux) {
    //On se positionne juste après SOS (traiter sos enregistre la position après les 3 octets non utile)
    fseek(image, position, SEEK_SET);

    size_t cap = 409006, len = 0;
    uint8_t *buf = malloc(cap); // buffer pour stocker le flux 
    if (!buf) {
        fprintf(stderr, "Erreur alloc mÃ©moire\n");
        return NULL;
    }

    int prev = 0;
    int cur;

    while ((cur = fgetc(image)) != EOF) {
        if (prev == 0xFF) {
            // Bytestuffing  (FF 00) on ignore 00 et conserve FF
            if (cur == 0x00) {
                prev = 0;
                continue;
            }
            // Restart markers (FFD0 à FFD7) on ignore l'octet
            if (cur >= 0xD0 && cur <= 0xD7) {
                prev = 0;
                continue;
            }
            // End Of Image : FFD9 c'est finiiii
            if (cur == 0xD9) {
                break;
            }
            
        }

        // Si besoin, redimensionner le buffer
        if (len >= cap) {
            cap <<= 1;
            uint8_t *tmp = realloc(buf, cap);
            if (!tmp) {
                free(buf);
                fprintf(stderr, "Erreur realloc\n");
                return NULL;
            }
            buf = tmp;
        }

        // L'octet courant est stocker 
        buf[len++] = (uint8_t)cur;
        prev = cur;
    }

    *taille_flux = len;
    return buf;
}



/* EXtraction de Y  grace à  Huffman + RLEinv sortie pls blocs 8 par 8 Y uniquement pour image noir et blanc*/
// input : bitstream, metadonnees, output : Vecteur de blocs Y
// output : tableau de float 8x8
float **extraire_blocs_y_noir_et_blanc(struct bitstream *stream, metadonnees_jpeg_t *meta)
{
    // On calcul le nombre d'blocs y
    uint64_t nb = ((meta->largeur  + 7) / 8) * ((meta->hauteur  + 7) / 8);

    // Construire UNE seule fois les tables de huffman sinon tout est cassé
    table_huffman_decode_t *huff_dc = construire_table_depuis_Li_symboles(
        meta->table_huffman_DC[meta->indice_huffman_Y_DC]->Li,
        meta->table_huffman_DC[meta->indice_huffman_Y_DC]->symboles
    );
    table_huffman_decode_t *huff_ac = construire_table_depuis_Li_symboles(
        meta->table_huffman_AC[meta->indice_huffman_Y_AC]->Li,
        meta->table_huffman_AC[meta->indice_huffman_Y_AC]->symboles
    );
    if (!huff_dc || !huff_ac) {
        fprintf(stderr, "Erreur lors de la création des tables de Huffman (on parle de Y)\n");
        exit(EXIT_FAILURE);
    }

    // extraction bloc par bloc
    float **blocs = malloc(nb * sizeof *blocs);
    if (!blocs) {
        fprintf(stderr, "Erreur d'allocation blocs Y\n");
        exit(EXIT_FAILURE);
    }
    int16_t dc_prev = 0;

    for (uint64_t i = 0; i < nb; i++) {
        // reset dc_prev après restart
        blocs[i] = malloc(64 * sizeof(float));
        if (!blocs[i]) {
            fprintf(stderr, "Erreur d'allocation blocs\n");
            exit(EXIT_FAILURE);
        }
        // décodage DC+AC dans vecteur ZigZag
        decoder_bloc(
            blocs[i],
            huff_dc,
            huff_ac,
            stream,
            &dc_prev
        );
    }

    // libération des arbres Huffman
    liberer_table_huffman(huff_dc);
    liberer_table_huffman(huff_ac);

    return blocs;
}

// Pour les couleurs : 


/* Constitution du nombre de mcu */

uint64_t nombre_mcu(int image_largeur, int image_hauteur, int h_max, int v_max) {
    int nb_mcu_x = (image_largeur  + (h_max * 8 - 1)) / (h_max * 8);
    int nb_mcu_y = (image_hauteur + (v_max * 8 - 1)) / (v_max * 8);
    return nb_mcu_x * nb_mcu_y;
}


/*Découper par MCU*/

MCU_t *decoupe_par_mcu(int64_t nb_mcus, struct bitstream *stream, metadonnees_jpeg_t *meta) {
    MCU_t *mcu_array = malloc(nb_mcus * sizeof(MCU_t));
    if (!mcu_array) {
        fprintf(stderr, "Erreur malloc mcu_array\n");
        exit(EXIT_FAILURE);
    }

    int16_t dc_prev_Y = 0, dc_prev_Cb = 0, dc_prev_Cr = 0;

    // Construire UNE seule fois les tables de huffman sinon tout est cassé
    table_huffman_decode_t *huff_dc_Y = construire_table_depuis_Li_symboles(
        meta->table_huffman_DC[meta->indice_huffman_Y_DC]->Li,
        meta->table_huffman_DC[meta->indice_huffman_Y_DC]->symboles);

    table_huffman_decode_t *huff_ac_Y = construire_table_depuis_Li_symboles(
        meta->table_huffman_AC[meta->indice_huffman_Y_AC]->Li,
        meta->table_huffman_AC[meta->indice_huffman_Y_AC]->symboles);

    table_huffman_decode_t *huff_dc_Cb = construire_table_depuis_Li_symboles(
        meta->table_huffman_DC[meta->indice_huffman_Cb_DC]->Li,
        meta->table_huffman_DC[meta->indice_huffman_Cb_DC]->symboles);

    table_huffman_decode_t *huff_ac_Cb = construire_table_depuis_Li_symboles(
        meta->table_huffman_AC[meta->indice_huffman_Cb_AC]->Li,
        meta->table_huffman_AC[meta->indice_huffman_Cb_AC]->symboles);

    table_huffman_decode_t *huff_dc_Cr = construire_table_depuis_Li_symboles(
        meta->table_huffman_DC[meta->indice_huffman_Cr_DC]->Li,
        meta->table_huffman_DC[meta->indice_huffman_Cr_DC]->symboles);

    table_huffman_decode_t *huff_ac_Cr = construire_table_depuis_Li_symboles(
        meta->table_huffman_AC[meta->indice_huffman_Cr_AC]->Li,
        meta->table_huffman_AC[meta->indice_huffman_Cr_AC]->symboles);

    for (int i = 0; i < nb_mcus; i++) {

        mcu_array[i] = creer_mcu(stream, meta,
                                 &dc_prev_Y, &dc_prev_Cb, &dc_prev_Cr,
                                 huff_dc_Y, huff_ac_Y,
                                 huff_dc_Cb, huff_ac_Cb,
                                 huff_dc_Cr, huff_ac_Cr);

        /* Test pour debug 
        if (i >= 4590 && i <= 4610) {
            printf("MCU %d, DC_Y = %.0f\n", i, mcu_array[i].blocs_Y[0][0]);
        }
        
        if (i == 4599 || i == 4600 || i == 4601) {
            printf("MCU %d : Y=%u blocs, Cb=%u, Cr=%u\n",
                i,
                meta->echantill_horizontal_Y * meta->echantill_vertical_Y,
                meta->echantill_horizontal_Cb * meta->echantill_vertical_Cb,
                meta->echantill_horizontal_Cr * meta->echantill_vertical_Cr);
        }
        */
    }

    // Libéréééé délivreeer
    liberer_table_huffman(huff_dc_Y);
    liberer_table_huffman(huff_ac_Y);
    liberer_table_huffman(huff_dc_Cb);
    liberer_table_huffman(huff_ac_Cb);
    liberer_table_huffman(huff_dc_Cr);
    liberer_table_huffman(huff_ac_Cr);

    return mcu_array;
}


MCU_t creer_mcu(struct bitstream *stream, metadonnees_jpeg_t *meta,
                int16_t *dc_prev_Y, int16_t *dc_prev_Cb, int16_t *dc_prev_Cr,
                table_huffman_decode_t *huff_dc_Y, table_huffman_decode_t *huff_ac_Y,
                table_huffman_decode_t *huff_dc_Cb, table_huffman_decode_t *huff_ac_Cb,
                table_huffman_decode_t *huff_dc_Cr, table_huffman_decode_t *huff_ac_Cr) {
    MCU_t mcu;

    mcu.blocs_Y = extraire_blocs_composante(stream, huff_dc_Y, huff_ac_Y,
                                            meta->echantill_horizontal_Y, meta->echantill_vertical_Y, dc_prev_Y);

    mcu.blocs_Cb = extraire_blocs_composante(stream, huff_dc_Cb, huff_ac_Cb,
                                             meta->echantill_horizontal_Cb, meta->echantill_vertical_Cb, dc_prev_Cb);

    mcu.blocs_Cr = extraire_blocs_composante(stream, huff_dc_Cr, huff_ac_Cr,
                                             meta->echantill_horizontal_Cr, meta->echantill_vertical_Cr, dc_prev_Cr);

    return mcu;
}



/* 
Chaque compo de le mcu est décoder
- Inpout: bistream
- Outpout: Matrice de bloc 8 par 8 par compo dim h*v
*/


float **extraire_blocs_composante(struct bitstream *stream, table_huffman_decode_t *huff_dc, table_huffman_decode_t *huff_ac, uint8_t h, uint8_t v, int16_t *dc_prev) {
    uint32_t nb_blocs = h * v;  /* Calcul très redondant iciii*/
    float **blocs = malloc(nb_blocs * sizeof(float*));
    if (!blocs) exit(EXIT_FAILURE);  
    if (nb_blocs > 4) {
        printf("problème nbr blocs");
        }
    for (uint32_t i = 0; i < nb_blocs; i++) {
        blocs[i] = calloc(64, sizeof(float));
        if (!blocs[i]) {
            fprintf(stderr, "Erreur dallocation bloc %u\n", i);
            // Libération partielle
            for (uint32_t j = 0; j < i; j++) free(blocs[j]);
            free(blocs);
            return NULL;
        }
        // On demande à huffman de décoder tout ça (output boc 8 par 8 décoder )
        decoder_bloc(blocs[i], huff_dc, huff_ac, stream, dc_prev);  
    }
    return blocs;
}
