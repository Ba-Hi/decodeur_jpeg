#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "../../include/quantification_inverse.h"

// Fonction pour vérifier si deux nombres flottants sont approximativement égaux
int float_equals(float a, float b, float epsilon) {
    return fabs(a - b) < epsilon;
}

// Test de la fonction quantification_inverse
void test_quantification_inverse() {
    printf("Test de quantification_inverse...\n");
    
    // Créer une table de quantification de test
    // Valeurs typiques pour une table de quantification JPEG
    uint16_t quant_table[64] = {
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99
    };
    
    // Créer un vecteur de coefficients de test
    float coefficients[64] = {
        -26, -3, -6, -4, -4, -2, -3, -3,
        0, -2, -4, -2, -5, -7, -6, -5,
        -3, -2, -5, -7, -6, -6, -7, -7,
        -2, -1, -4, -6, -7, -6, -7, -9,
        -3, -1, -1, -4, -6, -7, -7, -8,
        -2, -1, -1, -3, -4, -6, -7, -8,
        -2, -1, -1, -2, -3, -5, -6, -7,
        -2, -1, -1, -2, -3, -4, -5, -6
    };
    
    // Copier les coefficients pour pouvoir comparer avant/après
    float coefficients_copy[64];
    for (int i = 0; i < 64; i++) {
        coefficients_copy[i] = coefficients[i];
    }
    
    // Appliquer la quantification inverse
    quantification_inverse(coefficients, quant_table);
    
    // Vérifier que les coefficients ont été multipliés par les valeurs de la table
    for (int i = 0; i < 64; i++) {
        float expected = coefficients_copy[i] * quant_table[i];
        assert(float_equals(coefficients[i], expected, 0.001f));
    }
    
    printf("SUCCÈS: Test de quantification_inverse réussi\n");
}

// Test avec des valeurs extrêmes
void test_quantification_inverse_extreme_values() {
    printf("\nTest de quantification_inverse avec des valeurs extrêmes...\n");
    
    // Table de quantification avec des valeurs extrêmes
    uint16_t quant_table[64];
    for (int i = 0; i < 64; i++) {
        quant_table[i] = (i % 2 == 0) ? 1 : 255; // Alterner entre 1 et 255
    }
    
    // Coefficients avec des valeurs extrêmes
    float coefficients[64];
    for (int i = 0; i < 64; i++) {
        coefficients[i] = (i % 4 == 0) ? 0.0f : 
                         (i % 4 == 1) ? -128.0f : 
                         (i % 4 == 2) ? 127.0f : 
                         0.5f;
    }
    
    // Copier les coefficients pour pouvoir comparer avant/après
    float coefficients_copy[64];
    for (int i = 0; i < 64; i++) {
        coefficients_copy[i] = coefficients[i];
    }
    
    // Appliquer la quantification inverse
    quantification_inverse(coefficients, quant_table);
    
    // Vérifier que les coefficients ont été multipliés par les valeurs de la table
    for (int i = 0; i < 64; i++) {
        float expected = coefficients_copy[i] * quant_table[i];
        assert(float_equals(coefficients[i], expected, 0.001f));
    }
    
    printf("SUCCÈS: Test de quantification_inverse avec des valeurs extrêmes réussi\n");
}

// Test avec une table de quantification uniforme
void test_quantification_inverse_uniform() {
    printf("\nTest de quantification_inverse avec une table uniforme...\n");
    
    // Table de quantification uniforme
    uint16_t quant_table[64];
    for (int i = 0; i < 64; i++) {
        quant_table[i] = 10; // Toutes les valeurs sont 10
    }
    
    // Coefficients simples
    float coefficients[64];
    for (int i = 0; i < 64; i++) {
        coefficients[i] = (float)(i - 32); // Valeurs de -32 à 31
    }
    
    // Copier les coefficients pour pouvoir comparer avant/après
    float coefficients_copy[64];
    for (int i = 0; i < 64; i++) {
        coefficients_copy[i] = coefficients[i];
    }
    
    // Appliquer la quantification inverse
    quantification_inverse(coefficients, quant_table);
    
    // Vérifier que les coefficients ont été multipliés par 10
    for (int i = 0; i < 64; i++) {
        float expected = coefficients_copy[i] * 10.0f;
        assert(float_equals(coefficients[i], expected, 0.001f));
    }
    
    printf("SUCCÈS: Test de quantification_inverse avec une table uniforme réussi\n");
}

int main() {
    printf("\n=== Tests pour le module quantification_inverse ===\n");
    
    test_quantification_inverse();
    test_quantification_inverse_extreme_values();
    test_quantification_inverse_uniform();
    
    printf("Tous les tests ont été exécutés.\n");
    return 0;
}