#include "../include/huffman_decode.h"
#include "../include/bitstream.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Création d’un nœud Huffman
static table_huffman_decode_t* creer_noeud(enum node_type type, int8_t val) {
    
    table_huffman_decode_t *node = malloc(sizeof(table_huffman_decode_t));
    if (!node) return NULL;

    node->type = type;
    if (type == NODE) {
        node->u.node.left = NULL;
        node->u.node.right = NULL;
    } else {
        node->u.val = val;
    }
    return node;
}

static void inserer_code(table_huffman_decode_t *racine, uint16_t code, uint8_t taille, int8_t symbole) {
    table_huffman_decode_t *courant = racine;
    for (int i = taille - 1; i >= 0; i--) {
        uint8_t bit = (code >> i) & 1; // Récuperation du bit i
        if (bit == 0) {
            if (!courant->u.node.left)
                courant->u.node.left = creer_noeud(NODE, 0);
            courant = courant->u.node.left;
        } else {
            if (!courant->u.node.right)
                courant->u.node.right = creer_noeud(NODE, 0);
            courant = courant->u.node.right;
        }
    }

    if (taille <= 16) {
        uint16_t masque = (1 << taille) - 1; // 1 * taille
        if (code == masque) {
            fprintf(stderr, "Erreur : insertion d’un code Huffman composé uniquement de 1 (%u bits)\n", taille);
                }
    } // jamais de 11111..

    // Here we are at a LEAF
    courant->type = LEAF;
    courant->u.val = symbole;
}


table_huffman_decode_t* construire_table_depuis_Li_symboles(uint8_t Li[16], uint8_t* symboles) {
    table_huffman_decode_t *racine = creer_noeud(NODE, 0);
    if (!racine) return NULL;

    uint16_t code = 0;
    int index = 0;
    for (int i = 0; i < 16; i++) {
        uint8_t nb_codes = Li[i];
        for (int j = 0; j < nb_codes; j++) {
            inserer_code(racine, code, i + 1, symboles[index++]);
            code++;
        }
        code <<= 1; // On ajoute un bit pour les codes suivants
    }

    return racine;
}

// Décode un symbole Huffman à partir du flux
int8_t decoder_valeur_huffman(table_huffman_decode_t *table, struct bitstream *stream) {
    if (!table || !stream) return -1; // Erreur table ou flux NULL

    uint32_t bit; // Variable pour stocker le bit lu
    while (table && table->type == NODE) { // Tant que on est pas à une LEAF
        if (read_bitstream(stream, 1, &bit, false) != 1) return -1; // Erreur de lecture
        table = (bit & 1) ? table->u.node.right : table->u.node.left;
    }

    return table ? table->u.val : -1; // Retourne la valeur du symbole si trouvé, sinon -1
}



// Libère récursivement l’arbre Huffman
// Utilisé dans extraire_bloc.c
void liberer_table_huffman(table_huffman_decode_t *table) {
    if (!table) return;
    if (table->type == NODE) {
        liberer_table_huffman(table->u.node.left);
        liberer_table_huffman(table->u.node.right);
    }
    free(table);
}


// Récupère la vraie valeur signée à partir de sa catégorie et du bitstream
int16_t lire_valeur_reelle(uint8_t taille, struct bitstream *stream) {
    if (taille == 0) return 0;
    uint32_t bits;
    if (read_bitstream(stream, taille, &bits, false) == -1) {
        fprintf(stderr, "Erreur lors de la lecture du bitstream\n");
        return 0;
    }
    // MSB
    uint32_t msb = 1u << (taille - 1);
    if ((bits & msb) == 0) {
        // valeur négative
        return (int16_t)(bits - (1u << taille) + 1);
    } else {
        // valeur positive
        return (int16_t)bits;
    }
}

// Décode un bloc JPEG (DC + AC) dans un vecteur ZigZag
void decoder_bloc(float vecteur[64],
                  table_huffman_decode_t *arbre_dc,
                  table_huffman_decode_t *arbre_ac,
                  struct bitstream *stream,
                  int16_t *dernier_DC)
{
    // 1) DC
    int8_t cat_dc = decoder_valeur_huffman(arbre_dc, stream); // catégorie magnitude entre 0 et 11 = m

    int16_t diff_dc = lire_valeur_reelle((uint8_t)cat_dc, stream); // lit les m bit suivant
    int16_t val_dc = *dernier_DC + diff_dc;
    *dernier_DC = val_dc;
    vecteur[0] = (float)val_dc;

    // 2) AC
    int index = 1;
    while (index < 64) {
        int8_t symbole = decoder_valeur_huffman(arbre_ac, stream);

        if (symbole == 0x00) {
            // End Of Block : remplir le reste de zéros
            while (index < 64) {
                vecteur[index++] = 0.0f;
            }
            break;
        }
        if (symbole == (int8_t)0xF0) {
            // RLE : 16 zéros
            for (int k = 0; k < 16 && index < 64; k++) {
                vecteur[index++] = 0.0f;
            }
            continue;
        }
        // cas general
        // Run-length et magnitude
        // RLE : 4 bit haut(combien de zeros) + 4 bit bas (magnitude:combien de bit a lire pour le cof !=0)
        
        uint8_t run_zeros = (symbole >> 4) & 0x0F;
        uint8_t mag       = symbole & 0x0F;
        
        // insérer les zéros
        for (int k = 0; k < run_zeros && index < 64; k++) {
            vecteur[index++] = 0.0f;
        }
        
        // lire la valeur si mag>0
        if (mag > 0 && index < 64) {
            int16_t val = lire_valeur_reelle(mag, stream);
            vecteur[index++] = (float)val;
        }

    }
}