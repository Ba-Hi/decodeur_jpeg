#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../include/ecrire_ppm.h"
#include "../../include/YCbCr_2_RGB.h"

// Fonction pour vérifier le contenu d'un fichier PPM
int verify_ppm_file(const char* filename, int width, int height, Pixel** expected_image) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier PPM");
        return 0;
    }
    
    // Lire l'en-tête PPM
    char magic[3];
    int file_width, file_height, max_val;
    
    if (fscanf(file, "%2s\n%d %d\n%d\n", magic, &file_width, &file_height, &max_val) != 4) {
        fclose(file);
        return 0;
    }
    
    // Vérifier l'en-tête
    if (strcmp(magic, "P6") != 0 || file_width != width || file_height != height || max_val != 255) {
        fclose(file);
        return 0;
    }
    
    // Lire les données des pixels
    unsigned char* pixel_data = malloc(width * height * 3);
    if (!pixel_data) {
        fclose(file);
        return 0;
    }
    
    size_t bytes_read = fread(pixel_data, 1, width * height * 3, file);
    fclose(file);
    
    if ((int) bytes_read != width * height * 3) {
        free(pixel_data);
        return 0;
    }
    
    // Comparer avec les données attendues
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            if (pixel_data[index] != expected_image[y][x].r ||
                pixel_data[index + 1] != expected_image[y][x].g ||
                pixel_data[index + 2] != expected_image[y][x].b) {
                free(pixel_data);
                return 0;
            }
        }
    }
    
    free(pixel_data);
    return 1;
}

// Test de la fonction ecrire_image_ppm
void test_ecrire_image_ppm() {
    printf("Test de ecrire_image_ppm...\n");
    
    // Créer une petite image de test (4x4 pixels)
    const int width = 4;
    const int height = 4;
    
    Pixel** image = malloc(height * sizeof(Pixel*));
    for (int i = 0; i < height; i++) {
        image[i] = malloc(width * sizeof(Pixel));
    }
    
    // Remplir l'image avec un motif simple
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((x + y) % 2 == 0) {
                // Pixels blancs
                image[y][x].r = 255;
                image[y][x].g = 255;
                image[y][x].b = 255;
            } else {
                // Pixels noirs
                image[y][x].r = 0;
                image[y][x].g = 0;
                image[y][x].b = 0;
            }
        }
    }
    
    // Nom du fichier de test
    const char* test_file = "test_image.ppm";
    
    // Écrire l'image dans un fichier PPM
    ecrire_image_ppm(test_file, image, width, height);
    
    // Vérifier que le fichier a été correctement écrit
    assert(verify_ppm_file(test_file, width, height, image));
    
    printf("SUCCÈS: Test de ecrire_image_ppm réussi\n");
    
    // Nettoyage
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
    remove(test_file);
}

// Test avec une image de taille différente
void test_ecrire_image_ppm_different_size() {
    printf("Test de ecrire_image_ppm avec une image de taille différente...\n");
    
    // Créer une image de test (8x2 pixels)
    const int width = 8;
    const int height = 2;
    
    Pixel** image = malloc(height * sizeof(Pixel*));
    for (int i = 0; i < height; i++) {
        image[i] = malloc(width * sizeof(Pixel));
    }
    
    // Remplir l'image avec un dégradé de rouge
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            image[y][x].r = (x * 255) / (width - 1);
            image[y][x].g = 0;
            image[y][x].b = 0;
        }
    }
    
    // Nom du fichier de test
    const char* test_file = "test_image_gradient.ppm";
    
    // Écrire l'image dans un fichier PPM
    ecrire_image_ppm(test_file, image, width, height);
    
    // Vérifier que le fichier a été correctement écrit
    assert(verify_ppm_file(test_file, width, height, image));
    
    printf("SUCCÈS: Test de ecrire_image_ppm avec une image de taille différente réussi\n");
    
    // Nettoyage
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
    remove(test_file);
}

// Test avec une image colorée
void test_ecrire_image_ppm_color() {
    printf("Test de ecrire_image_ppm avec une image colorée...\n");
    
    // Créer une image de test (3x3 pixels)
    const int width = 3;
    const int height = 3;
    
    Pixel** image = malloc(height * sizeof(Pixel*));
    for (int i = 0; i < height; i++) {
        image[i] = malloc(width * sizeof(Pixel));
    }
    
    // Remplir l'image avec différentes couleurs
    // Rouge, Vert, Bleu
    // Cyan, Magenta, Jaune
    // Blanc, Noir, Gris
    
    // Ligne 1
    image[0][0].r = 255; image[0][0].g = 0;   image[0][0].b = 0;   // Rouge
    image[0][1].r = 0;   image[0][1].g = 255; image[0][1].b = 0;   // Vert
    image[0][2].r = 0;   image[0][2].g = 0;   image[0][2].b = 255; // Bleu
    
    // Ligne 2
    image[1][0].r = 0;   image[1][0].g = 255; image[1][0].b = 255; // Cyan
    image[1][1].r = 255; image[1][1].g = 0;   image[1][1].b = 255; // Magenta
    image[1][2].r = 255; image[1][2].g = 255; image[1][2].b = 0;   // Jaune
    
    // Ligne 3
    image[2][0].r = 255; image[2][0].g = 255; image[2][0].b = 255; // Blanc
    image[2][1].r = 0;   image[2][1].g = 0;   image[2][1].b = 0;   // Noir
    image[2][2].r = 128; image[2][2].g = 128; image[2][2].b = 128; // Gris
    
    // Nom du fichier de test
    const char* test_file = "test_image_color.ppm";
    
    // Écrire l'image dans un fichier PPM
    ecrire_image_ppm(test_file, image, width, height);
    
    // Vérifier que le fichier a été correctement écrit
    assert(verify_ppm_file(test_file, width, height, image));
    
    printf("SUCCÈS: Test de ecrire_image_ppm avec une image colorée réussi\n");
    
    // Nettoyage
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
    remove(test_file);
}

int main() {
    printf("=== Tests pour le module ecrire_ppm ===\n");
    
    test_ecrire_image_ppm();
    test_ecrire_image_ppm_different_size();
    test_ecrire_image_ppm_color();
    
    printf("Tous les tests ont été exécutés.\n");
    return 0;
}