#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
Processus inverse du sous-échantillonage (si fct existe) et réassemblage des blocs
Entrée:
Blocs: MCUs(complet) compo des blocs Y Cb Cr (ordre donné dans l'entete) sous échantilloné
Sortie:
Blocs: Bloc complet réassemblé avec Cb Cr redimenssioné
*/

float** up_C(float **bloc_c, int h_y, int v_y, int h_c, int v_c ){

    int larg_y = h_y*8;
    int haut_y = v_y*8; 
    int larg_c = h_c*8;
    int haut_c = v_c*8;

    /* Initialisation du bloc redimensionné */
    float **new_c = malloc(haut_y * sizeof(float *));
    if (new_c == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for new_c array\n");
        return NULL;
    }
    
    for (int i = 0; i < haut_y; i++) {
        new_c[i] = malloc(larg_y * sizeof(float));
        if (new_c[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for new_c row %d\n", i);
            // Freeeee again and again
            for (int j = 0; j < i; j++) {
                free(new_c[j]);
            }
            free(new_c);
            return NULL;
        }
        
        
        for (int j = 0; j < larg_y; j++) {
            new_c[i][j] = 0;
        }
    }

    int fct_h = h_y / h_c;
    int fct_v = v_y / v_c;
    

    // Debuug 
    if (bloc_c == NULL) {
        fprintf(stderr, "Erreur bloc_c  NULL  up_C\n");
        return new_c;  
    }

    for (int i = 0; i < haut_c; i++) {
        // Check if row is valid
        if (bloc_c[i] == NULL) {
            fprintf(stderr, "Erreur bloc_c colone%d  NULL\n", i);
            continue;
        }
        
        for (int j = 0; j < larg_c; j++) {
            float val = bloc_c[i][j];

            /* h redimensionnement horizontal et v redimensionnement vertical fonctionne si uniquement h ou v*/
            for (int v = 0; v < fct_v; v++) {
                for (int h = 0; h < fct_h; h++) {
                    int new_i = i * fct_v + v;
                    int new_j = j * fct_h + h;
                    
                    // On vérifie si tout va bien (niveau bord)
                    if (new_i < haut_y && new_j < larg_y) {
                        new_c[new_i][new_j] = val;
                    } else {
                        fprintf(stderr, "Attention niveau bords  new_i=%d, new_j=%d (max: %d, %d)\n", 
                                new_i, new_j, haut_y-1, larg_y-1);
                    }
                }
            }
        }
    }

    return new_c;
}

