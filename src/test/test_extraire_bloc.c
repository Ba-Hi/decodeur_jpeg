#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "../../include/bitstream.h"
#include "../../include/extraire_bitstream.h"
#include "../../include/huffman_decode.h"
#include "../../include/extraire_bloc.h"

// Affiche un bloc 8x8
void afficher_bloc(float *bloc) {
    for (int i = 0; i < 64; i++) {
        printf("%6.1f ", bloc[i]);
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n");
}

// Affiche un MCU complet (Y, Cb, Cr)
void afficher_mcu(MCU_t mcu, metadonnees_jpeg_t *meta, int index) {
    printf("=== MCU #%d ===\n", index);

    int nb_blocs_Y = meta->echantill_horizontal_Y * meta->echantill_vertical_Y;
    int nb_blocs_Cb = meta->echantill_horizontal_Cb * meta->echantill_vertical_Cb;
    int nb_blocs_Cr = meta->echantill_horizontal_Cr * meta->echantill_vertical_Cr;

    for (int i = 0; i < nb_blocs_Y; i++) {
        printf("Bloc Y[%d] :\n", i);
        afficher_bloc(mcu.blocs_Y[i]);
    }

    for (int i = 0; i < nb_blocs_Cb; i++) {
        printf("Bloc Cb[%d] :\n", i);
        afficher_bloc(mcu.blocs_Cb[i]);
    }

    for (int i = 0; i < nb_blocs_Cr; i++) {
        printf("Bloc Cr[%d] :\n", i);
        afficher_bloc(mcu.blocs_Cr[i]);
    }
}

// Libère un MCU
void liberer_mcu(MCU_t *mcu, metadonnees_jpeg_t *meta) {
    int nb_blocs_Y = meta->echantill_horizontal_Y * meta->echantill_vertical_Y;
    int nb_blocs_Cb = meta->echantill_horizontal_Cb * meta->echantill_vertical_Cb;
    int nb_blocs_Cr = meta->echantill_horizontal_Cr * meta->echantill_vertical_Cr;

    for (int i = 0; i < nb_blocs_Y; i++) free(mcu->blocs_Y[i]);
    for (int i = 0; i < nb_blocs_Cb; i++) free(mcu->blocs_Cb[i]);
    for (int i = 0; i < nb_blocs_Cr; i++) free(mcu->blocs_Cr[i]);

    free(mcu->blocs_Y);
    free(mcu->blocs_Cb);
    free(mcu->blocs_Cr);
}

// Test global : extraire blocs Y/Cb/Cr pour image JPEG donnée
void test_image_par_mcu(const char *fichier_image) {
    FILE *image = fopen(fichier_image, "rb");
    if (!image) {
        perror("Erreur ouverture image");
        return;
    }

    metadonnees_jpeg_t *meta = extraire_bitstream(image);
    size_t taille_flux;
    uint8_t *flux_donnees = lire_flux_donnees(image, meta->position_data, &taille_flux);

    meta->stream = create_bitstream(flux_donnees, taille_flux);
    fclose(image);

    int nb_mcus = nombre_mcu(meta->largeur, meta->hauteur,
                             meta->echantill_horizontal_Y,
                             meta->echantill_vertical_Y);

    printf("Nombre de MCUs à lire : %d\n", nb_mcus);

    MCU_t *mcus = decoupe_par_mcu(nb_mcus, meta->stream, meta);

    for (int i = 0; i < nb_mcus; i++) {
        afficher_mcu(mcus[i], meta, i);
        liberer_mcu(&mcus[i], meta);
    }
    free(mcus);
    free_bitstream(meta->stream);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s fichier.jpeg\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("=== TEST : Extraction blocs Y, Cb, Cr par MCU ===\n");
    test_image_par_mcu(argv[1]);
    printf("=== FIN DES TESTS ===\n");

    return EXIT_SUCCESS;
}
