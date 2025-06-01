#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "../include/idct.h"
#include "../include/idct_rapide.h"

#define TOLERANCE 0.1

// === Fonctions d'affichage ===

void afficher_matrice_int8(uint8_t*** mat) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%4u ", *mat[i][j]);
        }
        printf("\n");
    }
}

void afficher_matrice_float(float mat[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%3u ", (uint8_t) mat[i][j]);
        }
        printf("\n");
    }
}

void afficher_matrice_int16(int16_t*** mat) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%4d ", *mat[i][j]);
        }
        printf("\n");
    }
}

// === Allocation et libération dynamiques ===

int16_t*** allouer_matrice_int16() {
    int16_t*** mat = malloc(8 * sizeof(int16_t**));
    for (int i = 0; i < 8; i++) {
        mat[i] = malloc(8 * sizeof(int16_t*));
        for (int j = 0; j < 8; j++) {
            mat[i][j] = malloc(sizeof(int16_t));
        }
    }
    return mat;
}

uint8_t*** allouer_matrice_uint8() {
    uint8_t*** mat = malloc(8 * sizeof(uint8_t**));
    for (int i = 0; i < 8; i++) {
        mat[i] = malloc(8 * sizeof(uint8_t*));
        for (int j = 0; j < 8; j++) {
            mat[i][j] = malloc(sizeof(uint8_t));
        }
    }
    return mat;
}

void liberer_matrice_int16(int16_t*** mat) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            free(mat[i][j]);
        }
        free(mat[i]);
    }
    free(mat);
}

void liberer_matrice_uint8(uint8_t*** mat) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            free(mat[i][j]);
        }
        free(mat[i]);
    }
    free(mat);
}

// === Test unitaire principal ===

int tester_idct_unitaires() {
    int16_t*** in = allouer_matrice_int16();
    uint8_t*** out_rapide = allouer_matrice_uint8();
    float input[8][8] = {0}, reference[8][8] = {0};

    // Initialisation : seul le coefficient DC est à 100
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            *in[i][j] = 0;
            input[i][j] = 0.0f;
        }
    *in[0][0] = 100;
    input[0][0] = 100.0f;

    printf("\nMatrice d'entrée (DCT) :\n");
    afficher_matrice_int16(in);

    // IDCT mathématique
    idct_8x8(input, reference);

    // IDCT rapide
    idct_loeffler_2d(in, out_rapide);

    printf("\nRésultat IDCT mathématique :\n");
    afficher_matrice_float(reference);

    printf("\nRésultat IDCT rapide :\n");
    afficher_matrice_int8(out_rapide);

    // Comparaison
    int success = 1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            uint8_t attendu = (uint8_t) round(reference[i][j]);
            uint8_t obtenu = *out_rapide[i][j];
            int diff = abs(attendu - obtenu);
            if (diff > TOLERANCE) {
                printf("Erreur !! à [%d][%d] : attendu = %d, obtenu = %d\n", i, j, attendu, obtenu);
                success = 0;
            }
        }
    }

    liberer_matrice_int16(in);
    liberer_matrice_uint8(out_rapide);

    return success;
}

int main() {
    if (tester_idct_unitaires()) {
        printf("\n Test réussi : IDCT rapide ≈ IDCT mathématique\n");
    } else {
        printf("\n Test échoué\n");
    }

    return 0;
}
