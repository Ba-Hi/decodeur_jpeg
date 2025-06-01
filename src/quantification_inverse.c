#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void quantification_inverse(float vecteur[64], uint16_t* quant_table) {
    if (quant_table == NULL) {
        fprintf(stderr, "Erreur : la table de quantification est NULL\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 64; i++) {
        vecteur[i] *= quant_table[i];
    }
}
