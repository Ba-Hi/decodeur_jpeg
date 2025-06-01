#ifndef EXTRAIRE_BLOC_H
#define EXTRAIRE_BLOC_H


#include <stdint.h>
#include <stdio.h>
#include "bitstream.h"
#include "extraire_bitstream.h"
#include "huffman_decode.h"

// Structure représentant un MCU avec ses composantes Y, Cb, Cr
typedef struct {
    float **blocs_Y;
    float **blocs_Cb; 
    float **blocs_Cr; 
} MCU_t;

// Lit les données JPEG utiles après le marqueur SOS, avant EOI
uint8_t* lire_flux_donnees(FILE *f, long pos, size_t *taille_flux);

float **extraire_blocs_y_noir_et_blanc(struct bitstream *stream,
                         metadonnees_jpeg_t *meta);
                         
uint64_t nombre_mcu(int image_largeur, int image_hauteur, int h_max, int v_max);

MCU_t *decoupe_par_mcu(int64_t nb_mcus, struct bitstream *stream, metadonnees_jpeg_t *meta);

MCU_t creer_mcu(struct bitstream *stream, metadonnees_jpeg_t *meta,
                int16_t *dc_prev_Y, int16_t *dc_prev_Cb, int16_t *dc_prev_Cr,
                table_huffman_decode_t *huff_dc_Y, table_huffman_decode_t *huff_ac_Y,
                table_huffman_decode_t *huff_dc_Cb, table_huffman_decode_t *huff_ac_Cb,
                table_huffman_decode_t *huff_dc_Cr, table_huffman_decode_t *huff_ac_Cr);

float **extraire_blocs_composante(struct bitstream *stream,
                                  table_huffman_decode_t *huff_dc, table_huffman_decode_t *huff_ac,
                                  uint8_t h, uint8_t v, int16_t *dc_prev);


#endif
// EXTRAIRE_BLOC_H

