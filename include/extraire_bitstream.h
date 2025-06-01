#ifndef JPEG_METADATA_H
#define JPEG_METADATA_H

#include <stdint.h>
#include <stdio.h>

/* Représentation d'une table de Huffman */
typedef struct table_huffman {
    uint8_t Li[16];           // Nombre de codes de longueur i+1
    uint8_t* symboles;        // Liste des symboles
} table_huffman_t;

/* Stream de bits (déclaré ailleurs) */
typedef struct bitstream bitstream_t;

/* Structure principale contenant toutes les métadonnées JPEG */
typedef struct metadonnees_jpeg {
    /* Taille de l'image */
    uint16_t largeur;
    uint16_t hauteur;

    /* Tables de quantification */
    uint8_t  nb_tables_quantif;
    uint16_t* tables_quantif[4];
    uint8_t  precisions_quantif[4];

    /* Indices de quantification par composante */
    uint8_t indice_quantif_Y;
    uint8_t indice_quantif_Cb;
    uint8_t indice_quantif_Cr;

    /* Échantillonnage par composante */
    uint8_t echantill_horizontal_Y;
    uint8_t echantill_vertical_Y;
    uint8_t echantill_horizontal_Cb;
    uint8_t echantill_vertical_Cb;
    uint8_t echantill_horizontal_Cr;
    uint8_t echantill_vertical_Cr;

    /* Tables de Huffman */
    uint8_t       nb_tables_huffman_DC;
    uint8_t       nb_tables_huffman_AC;
    table_huffman_t** table_huffman_DC;
    table_huffman_t** table_huffman_AC;

    
    /* Index des tables Huffman utilisées (après SOS) */
    uint8_t indice_huffman_Y_DC, indice_huffman_Y_AC;
    uint8_t indice_huffman_Cb_DC, indice_huffman_Cb_AC;
    uint8_t indice_huffman_Cr_DC, indice_huffman_Cr_AC;


    uint8_t scan_0, scan_1, scan_2;

    /* Scan (SOS) */
    uint8_t nb_composantes_scan;         // Nombre de composantes dans le scan
    uint8_t ordre_composantes[3];        // IDs des composantes dans l'ordre du scan
    uint8_t indices_DC[3];               // Indices des tables DC pour chaque composante
    uint8_t indices_AC[3];               // Indices des tables AC pour chaque composante

    /* Restart interval */
    uint16_t restart_interval;

    /* Position et flux de données après SOS */
    long        position_data;           // Offset après le segment SOS
    bitstream_t* stream;                 // Bitstream des données d'image

    /* Nom de l'image d'origine (optionnel) */
    char* nom_image;
} metadonnees_jpeg_t;

/* Fonctions exposées pour l'extraction et le traitement des métadonnées */
metadonnees_jpeg_t* extraire_bitstream(FILE* image);
metadonnees_jpeg_t* init_metadonnes(void);
void traiter_sof0(FILE* image, metadonnees_jpeg_t* meta);
void traiter_dqt(FILE* image, metadonnees_jpeg_t* meta);
void traiter_dht(FILE* image, metadonnees_jpeg_t* meta);
void traiter_sos(FILE* image, metadonnees_jpeg_t* meta);
void traiter_dri(FILE* image, metadonnees_jpeg_t* meta);
void ignore_segment(FILE* image);
uint8_t lire_u8(FILE* image);
uint16_t lire_u16_big(FILE* image);

/* Utilities pour bitstream */
void bitstream_align_to_next_byte(bitstream_t* stream);
void bitstream_skip_restart_marker(bitstream_t* stream);

#endif // JPEG_METADATA_H

