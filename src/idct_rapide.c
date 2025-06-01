#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "../include/idct_rapide.h"

#define M_PI 3.14159265358979323846
#define M_SQRT2 1.4142135623730951
#define M_SQRT8 2.8284271247461903
#define CLAMP(x) ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

void butterfly(double* i0, double* i1, double o0, double o1) {
    *i0 = (o1 + o0) / 2;
    *i1 = (o0 - o1) / 2;
}

void inverse_rotation(double* i0, double* i1, double o0, double o1, double k, uint16_t n) {
    const double angle = (n * M_PI) / 16;
    const double COS = cos(angle);
    const double SIN = sin(angle);
    *i0 = (o0 * COS - o1 * SIN) / k;
    *i1 = (o1 * COS + o0 * SIN) / k;
}

void echanger(double*** vect, double*** vect2) {
    double** temp = *vect;
    *vect = *vect2;
    *vect2 = temp;
}

void loeffler_idct_1d(double** vector) {
    
    for (uint8_t i = 0; i < 8; i++) {
        *vector[i] *= M_SQRT8;
    }

    double* vect_stage_4 = malloc(8 * sizeof(double));

    // Stage 4
    butterfly(&vect_stage_4[7], &vect_stage_4[4], *vector[1], *vector[7]);
    vect_stage_4[5] = *vector[3] / M_SQRT2;
    vect_stage_4[6] = *vector[5] / M_SQRT2;
    vect_stage_4[0] = *vector[0];
    vect_stage_4[1] = *vector[4];
    vect_stage_4[2] = *vector[2];
    vect_stage_4[3] = *vector[6];

    double* vect_stage_3 = malloc(8 * sizeof(double));

    // Stage 3
    butterfly(&vect_stage_3[0], &vect_stage_3[1], vect_stage_4[0], vect_stage_4[1]);
    butterfly(&vect_stage_3[4], &vect_stage_3[6], vect_stage_4[4], vect_stage_4[6]);
    butterfly(&vect_stage_3[7], &vect_stage_3[5], vect_stage_4[7], vect_stage_4[5]);
    inverse_rotation(&vect_stage_3[2], &vect_stage_3[3], vect_stage_4[2], vect_stage_4[3], sqrt(2), 6);

    free(vect_stage_4);

    double* vect_stage_2 = malloc(8 * sizeof(double));

    // Stage 2 inversed
    butterfly(&vect_stage_2[0], &vect_stage_2[3], vect_stage_3[0], vect_stage_3[3]);
    butterfly(&vect_stage_2[1], &vect_stage_2[2], vect_stage_3[1], vect_stage_3[2]);
    inverse_rotation(&vect_stage_2[4], &vect_stage_2[7], vect_stage_3[4], vect_stage_3[7], 1, 3);
    inverse_rotation(&vect_stage_2[5], &vect_stage_2[6], vect_stage_3[5], vect_stage_3[6], 1, 1);

    free(vect_stage_3);

    // Stage 1 inversed
    butterfly(vector[0], vector[7], vect_stage_2[0], vect_stage_2[7]);
    butterfly(vector[1], vector[6], vect_stage_2[1], vect_stage_2[6]);
    butterfly(vector[2], vector[5], vect_stage_2[2], vect_stage_2[5]);
    butterfly(vector[3], vector[4], vect_stage_2[3], vect_stage_2[4]);

    free(vect_stage_2);
}

void transposee_matrice(double*** table) {
    double temp1, temp2;
    for (uint8_t i = 1; i < 8; i++) {
        for (uint8_t j = 0; j < i; j++) {
            temp1 = *table[i][j];
            temp2 = *table[j][i];
            *table[i][j] = temp2;
            *table[j][i] = temp1;
        }
    }
}

void idct_loeffler_2d(int16_t*** input_matrice, uint8_t*** out_matrice) {

    double*** bloc_dequantif_double = malloc(8 * sizeof(double**));
    for (int i = 0; i < 8; i++) {
        bloc_dequantif_double[i] = malloc(8 * sizeof(double*));
        for (int j = 0; j < 8; j++) {
            bloc_dequantif_double[i][j] = malloc(sizeof(double));
        }
    }

    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            *bloc_dequantif_double[i][j] = (double)*input_matrice[i][j];
        }
    }

    // idct appliquée sur les lignes
    for (uint8_t i = 0; i < 8; i++) {
        loeffler_idct_1d(bloc_dequantif_double[i]);
    }

    // transposition de la matrice pour appliquer encore idct sur les colonnes
    transposee_matrice(bloc_dequantif_double);

    // idct appliquée sur les colonnes
    for (uint8_t i = 0; i < 8; i++) {
        loeffler_idct_1d(bloc_dequantif_double[i]);
    }

    // t (t A) = A, on revient à l'état initial
    transposee_matrice(bloc_dequantif_double);


    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            *bloc_dequantif_double[i][j] = CLAMP(*bloc_dequantif_double[i][j] + 128);
            *out_matrice[i][j] = (uint8_t) round(*bloc_dequantif_double[i][j]);
        }
    }

    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            free(bloc_dequantif_double[i][j]);
        }
        free(bloc_dequantif_double[i]);
    }
    free(bloc_dequantif_double);
}
