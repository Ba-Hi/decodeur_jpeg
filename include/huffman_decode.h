#ifndef HUFFMAN_DECODE_H
#define HUFFMAN_DECODE_H

#include <stdint.h>
#include <stdbool.h>
#include "extraire_bitstream.h"

// Type de nœud
enum node_type {
    NODE,
    LEAF
};

// Définition de la structure d’arbre Huffman
struct huff_node { 
    enum node_type type;
    union { // Union pour stocker soit un nœud, soit une valeur, mais just one !
        struct { // Nœud interne
            struct huff_node *left;
            struct huff_node *right;
        } node;
        int8_t val;
    } u;
};


// Arbre de décodage (différent de la table JPEG brute)
typedef struct huff_node table_huffman_decode_t;

// Construit la table à partir d’un flux (bitstream JPEG brut)
table_huffman_decode_t* construire_table_huffman(struct bitstream *stream, uint16_t *nb_byte_read);

// Construit la table à partir de Li + symboles (JPEG brut)
table_huffman_decode_t* construire_table_depuis_Li_symboles(uint8_t Li[16], uint8_t* symboles);

// Décodage à partir de l’arbre
int8_t decoder_valeur_huffman(table_huffman_decode_t *table, struct bitstream *stream);

// Libération mémoire
void liberer_table_huffman(table_huffman_decode_t *table);

// Décoder sur place le vecteur
void decoder_bloc(float vecteur[64],
                  table_huffman_decode_t *arbre_dc,
                  table_huffman_decode_t *arbre_ac,
                  struct bitstream *stream,
                  int16_t *dernier_DC);
#endif // HUFFMAN_DECODE_H