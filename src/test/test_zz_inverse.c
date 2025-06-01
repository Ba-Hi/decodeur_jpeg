#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "zz_inverse.h"


// Affiche un bloc 8x8 de valeurs flottantes
void affiche(const char* titre, float bloc[8][8]) {
    printf("\n*** %s ***\n", titre);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%7.2f ", bloc[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Vérifie si le résultat du zigzag inverse est correct
 * en comparant avec les valeurs attendues
 */
int verifier_zigzag(float vecteur[64], float attendu[8][8]) {
    float resultat[8][8] = {0};
    zigzagInverse(vecteur, resultat);
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (resultat[i][j] != attendu[i][j]) {
                printf("ERREUR à la position [%d][%d]: attendu %.2f, obtenu %.2f\n", 
                       i, j, attendu[i][j], resultat[i][j]);
                return 0;
            }
        }
    }
    return 1;
}

int main(void) {

    int succes = 0;

    printf("=== TEST DE LA FONCTION ZIGZAG INVERSE ===\n");

    // Test 1: Valeurs séquentielles (0 à 63)
    printf("\nTest 1: Valeurs séquentielles (0 à 63)\n");
    float vect1[64];
    for (int i = 0; i < 64; i++) vect1[i] = i;

    float bloc1[8][8] = {0};
    zigzagInverse(vect1, bloc1);
    affiche("Résultat Test 1", bloc1);

    if (verifier_zigzag(vect1, bloc1)) {
        printf("Test 1: Tout va bien\n");
        succes++;
    } else {
        printf("Test 1: Tout va pas bien , ce n'est pas la sortie attendue\n");
    }

    // Test 2: Valeurs flottantes (multiples de 1.5)
    printf("\nTest 2: Valeurs flottantes (multiples de 1.5)\n");
    float vect2[64];
    for (int i = 0; i < 64; i++) vect2[i] = i * 1.5f;

    float bloc2[8][8] = {0};
    zigzagInverse(vect2, bloc2);
    affiche("Résultat Test 2", bloc2);


    if (verifier_zigzag(vect2, bloc2)) {
        printf("Test 2: Tout va bien\n");
        succes++;
    } else {
        printf("Test 2: Tout ne va pas bien , ce n'est pas la sortie attendue\n");
    }

    // Test 3: Valeurs aléatoires non ordonnées
    printf("\nTest 3: Valeurs aléatoires non ordonnées\n");
    float vect3[64] = {
        12.5, -3.2, 45.0, 0.0, 7.1, -15.4, 23.8, 5.5,
        9.3, 42.7, -8.6, 3.3, -1.9, 17.0, 30.2, -4.8,
        6.9, -12.0, 18.6, 25.1, -9.5, 14.7, 33.0, -6.2,
        11.4, -20.5, 2.2, 28.9, 4.1, -13.8, 19.7, -7.3,
        21.5, -5.9, 0.8, 16.3, -2.4, 8.0, 27.6, -11.7,
        3.9, 34.5, -10.8, 13.2, -16.5, 24.0, 1.7, -19.1,
        29.4, -14.2, 20.8, -3.5, 15.6, -17.9, 26.3, -18.4,
        31.7, -21.6, 22.5, -22.8, 32.1, -23.3, 35.9, -24.7
    };

    float bloc3[8][8] = {0};
    zigzagInverse(vect3, bloc3);
    affiche("Résultat Test 3", bloc3);

    if (verifier_zigzag(vect3, bloc3)) {
        printf("Test 3: Tout va bien\n");
        succes++;
    } else {
        printf("Test 3: Tout ne va pas bien , ce n'est pas la sortie attendue\n");
    }

    // Test 4: Vérification avec valeurs connues
    printf("\nTest 4: Vérification avec valeurs connues\n");
    float vect4[64] = {
        10, 20, 30, 40, 50, 60, 70, 80,
        90, 100, 110, 120, 130, 140, 150, 160,
        170, 180, 190, 200, 210, 220, 230, 240,
        250, 260, 270, 280, 290, 300, 310, 320,
        330, 340, 350, 360, 370, 380, 390, 400,
        410, 420, 430, 440, 450, 460, 470, 480,
        490, 500, 510, 520, 530, 540, 550, 560,
        570, 580, 590, 600, 610, 620, 630, 640
    };

    float attendu4[8][8] = {
        {10, 20, 60, 70, 150, 160, 280, 290},
        {30, 50, 80, 140, 170, 270, 300, 430},
        {40, 90, 130, 180, 260, 310, 420, 440},
        {100, 120, 190, 250, 320, 410, 450, 540},
        {110, 200, 240, 330, 400, 460, 530, 550},
        {210, 230, 340, 390, 470, 520, 560, 610},
        {220, 350, 380, 480, 510, 570, 600, 620},
        {360, 370, 490, 500, 580, 590, 630, 640}
    };

    float bloc4[8][8] = {0};
    zigzagInverse(vect4, bloc4);
    affiche("Résultat Test 4", bloc4);

    if (verifier_zigzag(vect4, attendu4)) {
        printf("Test 4: Tout va bien\n");
        succes++;
    } else {
        printf("Test 4: Tout ne va pas bien , ce n'est pas la sortie attendue\n");
    }

    printf("\n === TESTS ZIGZAG TERMINÉS=== \n");
    
    if (succes == 4) {
        printf("Zig-zag focntionnne !\n");
    } else {
        printf("Au moins un test a échoué.\n");
    }
    return 0;
}
