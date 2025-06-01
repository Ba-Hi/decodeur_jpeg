#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "YCbCr_2_RGB_nb.h" // Contient la fonction convert_ycbcr_2_rgb_noir_et_blanc

/* Fonction de création de bloc */
float **creer_bloc(int taille, float val) {
    float **bloc = malloc(taille * sizeof(float *));
    for (int i = 0; i < taille; i++) {
        bloc[i] = malloc(taille * sizeof(float));
        for (int j = 0; j < taille; j++) {
            bloc[i][j] = val;
        }
    }
    return bloc;
}

/* Libère un bloc de floats */
void liberer_bloc_float(float **bloc, int taille) {
    for (int i = 0; i < taille; i++) {
        free(bloc[i]);
    }
    free(bloc);
}

/* Libère un bloc de Pixels */
void liberer_bloc_pixel(Pixel **bloc, int taille) {
    for (int i = 0; i < taille; i++) {
        free(bloc[i]);
    }
    free(bloc);
}

/* Affiche un bloc de pixels RGB */
void afficher_bloc_rgb(Pixel **bloc, int taille) {
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            printf("(%3d,%3d,%3d) ", bloc[i][j].r, bloc[i][j].g, bloc[i][j].b);
        }
        printf("\n");
    }
}

int main(void) {
    printf("=== TEST CONVERSION Y -> NOIR ET BLANC ===\n");

    int taille = 4;

    /* Test : Valeur fixe à 100 (gris) */
    float **Y = creer_bloc(taille, 100);
    printf("\nBloc Y d'entrée (100 partout) :\n");
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            printf("%6.1f ", Y[i][j]);
        }
        printf("\n");
    }

    Pixel **RGB = convert_ycbcr_2_rgb_noir_et_blanc(Y, taille);
    printf("\nBloc RGB converti :\n");
    afficher_bloc_rgb(RGB, taille);

    /* Vérification simple */
    int erreurs = 0;
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            if (RGB[i][j].r != 100 || RGB[i][j].g != 100 || RGB[i][j].b != 100) {
                erreurs++;
            }
        }
    }

    if (erreurs == 0) {
        printf("\nTest réussi\n");
    } else {
        printf("\nTest échoué - %d erreurs détectées\n", erreurs);
    }

    liberer_bloc_float(Y, taille);
    liberer_bloc_pixel(RGB, taille);

    return 0;
}
