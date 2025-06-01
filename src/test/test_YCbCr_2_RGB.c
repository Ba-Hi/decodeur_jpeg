#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "YCbCr_2_RGB.h"

/* Alloue un bloc (hauteur x largeur) et l'initialise avec une valeur */ 
float **creer_bloc(int hauteur, int largeur, float valeur_initiale) {
    float **bloc = (float **)malloc(hauteur * sizeof(float *));
    if (!bloc) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les lignes.\n");
        return NULL;
    }

    for (int i = 0; i < hauteur; i++) {
        bloc[i] = (float *)malloc(largeur * sizeof(float));
        if (!bloc[i]) {
            fprintf(stderr, "Erreur d'allocation mémoire pour la ligne %d.\n", i);
            for (int j = 0; j < i; j++) free(bloc[j]);
            free(bloc);
            return NULL;
        }

        for (int j = 0; j < largeur; j++) {
            bloc[i][j] = valeur_initiale;
        }
    }

    return bloc;
}



/*
Libère la mémoire d'un bloc float
 */
void liberer_bloc_float(float **bloc, int hauteur) {
    if (bloc) {
        for (int i = 0; i < hauteur; i++) {
            if (bloc[i]) free(bloc[i]);
        }
        free(bloc);
    }
}

/*
Libère la mémoire d'un bloc Pixel
 */
void liberer_bloc_pixel(Pixel **bloc,  int hauteur) {
    if (bloc) {
        for (int i = 0; i < hauteur; i++) {
            if (bloc[i]) free(bloc[i]);
        }
        free(bloc);
    }
}

/**
 * Affiche un bloc YCbCr
 */
void afficher_bloc_ycbcr(const char* titre, float **Y, float **Cb, float **Cr, int hauteur, int largeur) {
    printf("\n=== %s ===\n", titre);
    
    printf("Y:\n");
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            printf("%7.2f ", Y[i][j]);
        }
        printf("\n");
    }
    
    printf("\nCb:\n");
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            printf("%7.2f ", Cb[i][j]);
        }
        printf("\n");
    }
    
    printf("\nCr:\n");
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            printf("%7.2f ", Cr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Implémentation de la fonction afficher_bloc_rgb déclarée dans le header
 */
void afficher_bloc_rgb(Pixel **RGB, int hauteur, int largeur) {
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            printf("(%3d,%3d,%3d) ", RGB[i][j].r, RGB[i][j].g, RGB[i][j].b);
        }
        printf("\n");
    }
}

/**
 * Wrapper pour afficher un bloc RGB avec un titre
 */
void afficher_bloc_rgb_avec_titre(const char* titre, Pixel **RGB, int hauteur, int largeur) {
    printf("\n=== %s ===\n", titre);
    afficher_bloc_rgb(RGB, hauteur,largeur);
    printf("\n");
}

/**
 * Vérifie si la conversion YCbCr -> RGB est correcte
 */
int verifier_conversion(float **Y, float **Cb, float **Cr, Pixel **RGB, int hauteur, int largeur) {
    int erreurs = 0;
    
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            // Calcul des valeurs RGB attendues
            int r_attendu = (int)(Y[i][j] + 1.402 * (Cr[i][j] - 128));
            int g_attendu = (int)(Y[i][j] - 0.34414 * (Cb[i][j] - 128) - 0.71414 * (Cr[i][j] - 128));
            int b_attendu = (int)(Y[i][j] + 1.772 * (Cb[i][j] - 128));
            
            // Saturation entre 0 et 255
            if (r_attendu < 0) r_attendu = 0;
            if (r_attendu > 255) r_attendu = 255;
            if (g_attendu < 0) g_attendu = 0;
            if (g_attendu > 255) g_attendu = 255;
            if (b_attendu < 0) b_attendu = 0;
            if (b_attendu > 255) b_attendu = 255;
            
            // Vérification
            if (RGB[i][j].r != r_attendu || RGB[i][j].g != g_attendu || RGB[i][j].b != b_attendu) {
                printf("ERREUR à la position [%d][%d]:\n", i, j);
                printf("  Attendu: R=%d, G=%d, B=%d\n", r_attendu, g_attendu, b_attendu);
                printf("  Obtenu:  R=%d, G=%d, B=%d\n", RGB[i][j].r, RGB[i][j].g, RGB[i][j].b);
                erreurs++;
            }
        }
    }
    
    return (erreurs == 0);
}

int main(void) { 
    printf("=== TEST DE LA FONCTION CONVERT_YCBCR_2_RGB ===\n");
    
    int hauteur = 4; 
    int largeur = 4;

    /* Test 1 : Valeur fixe à 100 (gris) */
    printf("\nTest 1: Gris (Y=100, Cb=Cr=128)\n");

    float **Y1 = creer_bloc(hauteur, largeur, 100);
    float **Cb1 = creer_bloc(hauteur, largeur, 128);
    float **Cr1 = creer_bloc(hauteur, largeur, 128);

    afficher_bloc_ycbcr("Entrée YCbCr - Test 1", Y1, Cb1, Cr1, hauteur, largeur);
    Pixel **RGB1 = convert_ycbcr_2_rgb(Y1, Cb1, Cr1, hauteur, largeur);
    afficher_bloc_rgb_avec_titre("Sortie RGB - Test 1", RGB1, hauteur, largeur);
    
    if (verifier_conversion(Y1, Cb1, Cr1, RGB1, hauteur, largeur)) {
        printf("Test 1:Tout va bien\n");
    } else {
        printf("Test 1: Tout va mal\n");
    }
    
    liberer_bloc_float(Y1, hauteur);
    liberer_bloc_float(Cb1, hauteur);
    liberer_bloc_float(Cr1, hauteur);
    liberer_bloc_pixel(RGB1, hauteur);

    /* Test 2 : Luminance croissante */
    printf("\nTest 2: Luminance croissante (Y variable, Cb=Cr=128)\n");

    float **Y2 = creer_bloc(hauteur, largeur, 0);
    float **Cb2 = creer_bloc(hauteur, largeur, 128);
    float **Cr2 = creer_bloc(hauteur, largeur, 128);
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            Y2[i][j] = i *hauteur + j * 20;
            if (Y2[i][j] > 255) Y2[i][j] = 255;
        }
    }

    afficher_bloc_ycbcr("Entrée YCbCr - Test 2", Y2, Cb2, Cr2, hauteur, largeur);
    Pixel **RGB2 = convert_ycbcr_2_rgb(Y2, Cb2, Cr2, hauteur, largeur);
    afficher_bloc_rgb_avec_titre("Sortie RGB - Test 2", RGB2, hauteur, largeur);
    
    if (verifier_conversion(Y2, Cb2, Cr2, RGB2, hauteur, largeur)) {
        printf("Test 2: SUCCÈS - La conversion est correcte\n");
    } else {
        printf("Test 2: ÉCHEC - La conversion contient des erreurs\n");
    }
    
    liberer_bloc_float(Y2, hauteur);
    liberer_bloc_float(Cb2, hauteur);
    liberer_bloc_float(Cr2, hauteur);
    liberer_bloc_pixel(RGB2, hauteur);
    
    /* Test 3 : Valeurs quelconques */
    printf("\nTest 3: Valeurs quelconques\n");

    float **Y3 = creer_bloc(hauteur, largeur, 90);
    float **Cb3 = creer_bloc(hauteur, largeur, 128);
    float **Cr3 = creer_bloc(hauteur, largeur, 128);
    Y3[0][0] = 50;
    Y3[1][1] = 200;
    Cb3[0][0] = 140;
    Cr3[1][1] = 160;

    afficher_bloc_ycbcr("Entrée YCbCr - Test 3", Y3, Cb3, Cr3, hauteur, largeur);
    Pixel **RGB3 = convert_ycbcr_2_rgb(Y3, Cb3, Cr3, hauteur, largeur);
    afficher_bloc_rgb_avec_titre("Sortie RGB - Test 3", RGB3, hauteur, largeur);
    
    if (verifier_conversion(Y3, Cb3, Cr3, RGB3, hauteur, largeur)) {
        printf("Test 3: Tout va bien\n");
    } else {
        printf("Test 3: échec\n");
    }
    
    liberer_bloc_float(Y3, hauteur);
    liberer_bloc_float(Cb3, hauteur);
    liberer_bloc_float(Cr3, hauteur);
    liberer_bloc_pixel(RGB3, hauteur);

    /* Test 4 : Couleurs primaires aux coins */
    printf("\nTest 4: Couleurs primaires aux coins\n");
    
    float **Y4 = creer_bloc(hauteur, largeur, 128);
    float **Cb4 = creer_bloc(hauteur, largeur, 128);
    float **Cr4 = creer_bloc(hauteur, largeur, 128);
    
    // Coin supérieur gauche: rouge (Y=76, Cb=85, Cr=255)
    Y4[0][0] = 76;
    Cb4[0][0] = 85;
    Cr4[0][0] = 255;
    
    // Coin supérieur droit: vert (Y=149, Cb=44, Cr=21)
    Y4[0][largeur-1] = 149;
    Cb4[0][largeur-1] = 44;
    Cr4[0][largeur-1] = 21;
    
    // Coin inférieur gauche: bleu (Y=29, Cb=255, Cr=107)
    Y4[hauteur-1][0] = 29;
    Cb4[hauteur-1][0] = 255;
    Cr4[hauteur-1][0] = 107;
    
    // Coin inférieur droit: jaune (Y=226, Cb=0, Cr=149)
    Y4[hauteur-1][largeur-1] = 226;
    Cb4[hauteur-1][largeur-1] = 0;
    Cr4[hauteur-1][largeur-1] = 149;
    
    afficher_bloc_ycbcr("Entrée YCbCr - Test 4", Y4, Cb4, Cr4, hauteur, largeur);
    Pixel **RGB4 = convert_ycbcr_2_rgb(Y4, Cb4, Cr4, hauteur, largeur);
    afficher_bloc_rgb_avec_titre("Sortie RGB - Test 4", RGB4, hauteur, largeur);
    
    if (verifier_conversion(Y4, Cb4, Cr4, RGB4, hauteur, largeur)) {
        printf("Test 4: Test réussi\n");
    } else {
        printf("Test 4: Noooon des erreurs\n");
    }
    
    liberer_bloc_float(Y4, hauteur);
    liberer_bloc_float(Cb4,hauteur);
    liberer_bloc_float(Cr4, hauteur);
    liberer_bloc_pixel(RGB4, hauteur);

    /* Test 5 : Invader */
    printf("\nTest 5: Invader\n");

    hauteur = 8; 
    largeur = 8; 
    float **Y5 = creer_bloc(hauteur, largeur, 0);
    float valeurs_Y[8][8] = {
        {0, 0, 0, 255, 255, 0, 0, 0},
        {0, 0, 255, 255, 255, 255, 0, 0},
        {0, 255, 255, 255, 255, 255, 255, 0},
        {255, 255, 0, 255, 255, 0, 255, 255},
        {255, 255, 255, 255, 255, 255, 255, 255},
        {0, 0, 255, 0, 0, 255, 0, 0},
        {0, 255, 0, 255, 255, 0, 255, 0},
        {255, 0, 255, 0, 0, 255, 0, 255}
    };
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Y5[i][j] = valeurs_Y[i][j];
        }
    }

    float **Cb5 = creer_bloc(hauteur, largeur, 128);
    float **Cr5 = creer_bloc(hauteur, largeur, 128);

    afficher_bloc_ycbcr("Entrée YCbCr - Test 5", Y5, Cb5, Cr5, hauteur, largeur);
    Pixel **RGB5 = convert_ycbcr_2_rgb(Y5, Cb5, Cr5, hauteur, largeur);
    afficher_bloc_rgb_avec_titre("Sortie RGB - Test 5", RGB5, hauteur, largeur);
    
    if (verifier_conversion(Y5, Cb5, Cr5, RGB5, hauteur, largeur)) {
        printf("Test 5: Test réussi\n");
    } else {
        printf("Test 5: test échoué malheureusement\n");
    }
    
    liberer_bloc_float(Y5,hauteur);
    liberer_bloc_float(Cb5, hauteur);
    liberer_bloc_float(Cr5, hauteur);
    liberer_bloc_pixel(RGB5, hauteur);

    printf("\n ===Test YCbCr_2_RGB finiii=== \n");


    return 0;
}
