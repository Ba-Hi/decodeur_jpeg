#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "extraire_bitstream.h"
#include "extraire_bloc.h"
#include "bitstream.h"
#include "huffman_decode.h"
#include "quantification_inverse.h"
#include "zz_inverse.h"
#include "idct.h"
#include "YCbCr_2_RGB.h"
#include "up_sampling.h"
#include "traiter_image_noir_blanc.h"
#include "traiter_image_couleur.h"


void liberer_metadonnees(metadonnees_jpeg_t *meta) {
    if (!meta) return;

    for (int i = 0; i < 3; i++) {
        if (meta->table_huffman_DC[i]) {
            free(meta->table_huffman_DC[i]->symboles);
            free(meta->table_huffman_DC[i]);
            meta->table_huffman_DC[i] = NULL;
        }
        if (meta->table_huffman_AC[i]) {
            free(meta->table_huffman_AC[i]->symboles);
            free(meta->table_huffman_AC[i]);
            meta->table_huffman_AC[i] = NULL;
        }
        if (meta->tables_quantif[i]) {
            free(meta->tables_quantif[i]);
            meta->tables_quantif[i] = NULL;
        }
    }

    if (meta->table_huffman_DC) free(meta->table_huffman_DC);
    if (meta->table_huffman_AC) free(meta->table_huffman_AC);

    if (meta->stream) {
        free_bitstream(meta->stream);
        meta->stream = NULL;
    }
    free(meta);

    // if (meta->nom_image) {
    //     free(meta->nom_image);
    // }
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <nom_fichier_jpeg>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int use_loeffler = 1; // Par défaut, on utilise l'algorithme de Loeffler
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--loeffler") == 0) {
            use_loeffler = 1;
        } else if (strcmp(argv[i], "--classic") == 0) {
            use_loeffler = 0;
        } else {
            fprintf(stderr, "Option inconnue : %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    FILE *image = fopen(argv[1], "rb");
    if (!image) {
        perror("Erreur lors de l'ouverture du fichier JPEG");
        return EXIT_FAILURE;
    }

    /*Extraction des métadonnées*/
    metadonnees_jpeg_t *meta = extraire_bitstream(image);
    if (!meta) {
        fprintf(stderr, "Erreur lors de la lecture des métadonnées\n");
        fclose(image);
        return EXIT_FAILURE;
    }
    
    /*Pour connaître la taille totale des données*/  
    size_t taille_flux;
    uint8_t *flux_donnees = lire_flux_donnees(image, meta->position_data, &taille_flux);
    if (!flux_donnees) {
        fprintf(stderr, "Erreur lors de la lecture des données\n");
        fclose(image);
        liberer_metadonnees(meta);
        return EXIT_FAILURE;
    }
    fclose(image);
    
    meta->stream = create_bitstream(flux_donnees, taille_flux);

    if (!meta->stream) {
        fprintf(stderr, "Erreur lors de la création du bitstream.\n");
        free(flux_donnees);
        return EXIT_FAILURE;
    }

    free(flux_donnees);

    /*on différencie les cas selon le type de l'imageIci*/
    if (meta->nb_composantes_scan == 1){
        traiter_image_noir_blanc(meta, argv[1], use_loeffler);
    } else if (meta->nb_composantes_scan == 3) {
        traiter_image_couleur(meta, argv[1], use_loeffler); 
    } else {
        printf("L'image ne contient pas un nombre valide de composantes. %d composants.\n", meta->nb_composantes_scan);
        printf("Traitement de l'image impossible. \n");
        return EXIT_FAILURE;
    }


    liberer_metadonnees(meta);
    return EXIT_SUCCESS;
}





