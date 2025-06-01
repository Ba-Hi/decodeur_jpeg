#include <stdio.h>
#include "ecrire_ppm.h"


void ecrire_image_ppm(const char *nom_fichier, Pixel **image, int largeur, int hauteur) {
    
    
    FILE *f = fopen(nom_fichier, "wb");
    if (!f) {
        perror("Erreur ouverture fichier PPM");
        return;
    }

    fprintf(f, "P6\n%d %d\n255\n", largeur, hauteur);

    // Vérifier que l'image n'est pas NULL
    if (!image) {
        printf("ERREUR: Pointeur image NULL!\n");
        fclose(f);
        return;
    }

    for (int y = 0; y < hauteur; y++) {
        // Vérifier que la ligne n'est pas NULL
        if (!image[y]) {
            printf("ERREUR: Ligne %d est NULL!\n", y);
            fclose(f);
            return;
        }
        for (int x = 0; x < largeur; x++) {
            // Écrire les valeurs RGB
            fputc(image[y][x].r, f);
            fputc(image[y][x].g, f);
            fputc(image[y][x].b, f);
            
            // Vérifier si on a écrit correctement (tous les 1000 pixels pour ne pas ralentir)
            if ((y * largeur + x) % 1000 == 0) {
                if (ferror(f)) {
                    printf("ERREUR d'écriture à la position (%d,%d)\n", x, y);
                    clearerr(f);
                }
            }
        }
        
    }

    fclose(f);
}

void ecrire_image_pgm(const char *nom_fichier, Pixel **image, int largeur, int hauteur) {
    
    
    FILE *f = fopen(nom_fichier, "wb");
    if (!f) {
        perror("Erreur ouverture fichier PPM");
        return;
    }

    fprintf(f, "P5\n%d %d\n255\n", largeur, hauteur);

    // Vérifier que l'image n'est pas NULL
    if (!image) {
        printf("ERREUR: Pointeur image NULL!\n");
        fclose(f);
        return;
    }

    for (int y = 0; y < hauteur; y++) {
        // Vérifier que la ligne n'est pas NULL
        if (!image[y]) {
            printf("ERREUR: Ligne %d est NULL!\n", y);
            fclose(f);
            return;
        }
        for (int x = 0; x < largeur; x++) {
            // Écrire les valeurs RGB
            fputc(image[y][x].r, f);
            
            // Vérifier si on a écrit correctement (tous les 1000 pixels pour ne pas ralentir)
            if ((y * largeur + x) % 1000 == 0) {
                if (ferror(f)) {
                    printf("ERREUR d'écriture à la position (%d,%d)\n", x, y);
                    clearerr(f);
                }
            }
        }
        
    }

    fclose(f);
}