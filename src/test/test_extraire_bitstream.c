#include <stdio.h>
#include <stdlib.h>
#include "../../include/extraire_bitstream.h"

// Ligne horizontale pour tableaux
void print_separator(int len) {
    for (int i = 0; i < len; i++) printf("-");
    printf("\n");
}

void afficher_table_quantif(uint16_t* table) {
    for (int i = 0; i < 64; i++) {
        printf("%3d ", table[i]);
        if ((i + 1) % 8 == 0) printf("\n");
    }
}

void afficher_table_huffman(table_huffman_t* table) {
    if (!table) {
        printf("  (table vide)\n");
        return;
    }

    printf("  Longueurs de codes (Li) :\n    ");
    for (int i = 0; i < 16; i++) {
        printf("%2d ", table->Li[i]);
    }
    printf("\n");

    /*
    printf("  Symboles (%d total) :\n    ", table->nb_symboles);
    for (int i = 0; i < table->nb_symboles; i++) {
        printf("%02X ", table->symboles[i]);
        if ((i + 1) % 16 == 0) printf("\n    ");
    }
    printf("\n");
    */ 
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s fichier.jpg\n", argv[0]);
        return 1;
    }

    FILE *image = fopen(argv[1], "rb");
    if (!image) {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    metadonnees_jpeg_t *meta = extraire_bitstream(image);
    fclose(image);

    if (!meta) {
        fprintf(stderr, "Erreur lors de l’extraction des métadonnées JPEG.\n");
        return 1;
    }

    printf("MÉTADONNÉES JPEG PARSÉES");

    // Dimensions
    print_separator(50);
    printf("| %-20s | %25s |\n", "Champ", "Valeur");
    print_separator(50);
    printf("| %-20s | %25dx%d |\n", "Dimensions", meta->largeur, meta->hauteur);
    printf("| %-20s | %25d |\n", "Tables de quantif", meta->nb_tables_quantif);
    printf("| %-20s | %13d AC / %d DC |\n", "Tables Huffman", meta->nb_tables_huffman_AC, meta->nb_tables_huffman_DC);
    print_separator(50);

    // Composantes
    printf("COMPOSANTES DE L’IMAGE (SOF0 + SOS)");
    print_separator(80);
    printf("| %-10s | %-8s | %-10s | %-10s | %-10s | %-10s |\n",
           "Composante", "Quantif", "Huff DC", "Huff AC", "H échant.", "V échant.");
    print_separator(80);
    printf("| %-10s | %-8d | %-10d | %-10d | %-10d | %-10d |\n",
           "Y", meta->indice_quantif_Y, meta->indice_huffman_Y_DC, meta->indice_huffman_Y_AC,
           meta->echantill_horizontal_Y, meta->echantill_vertical_Y);
    printf("| %-10s | %-8d | %-10d | %-10d | %-10d | %-10d |\n",
           "Cb", meta->indice_quantif_Cb, meta->indice_huffman_Cb_DC, meta->indice_huffman_Cb_AC,
           meta->echantill_horizontal_Cb, meta->echantill_vertical_Cb);
    printf("| %-10s | %-8d | %-10d | %-10d | %-10d | %-10d |\n",
           "Cr", meta->indice_quantif_Cr, meta->indice_huffman_Cr_DC, meta->indice_huffman_Cr_AC,
           meta->echantill_horizontal_Cr, meta->echantill_vertical_Cr);
    print_separator(80);

    // Tables de quantification
    printf("TABLES DE QUANTIFICATION");
    for (int i = 0; i < 4; i++) {
        if (meta->tables_quantif[i]) {
            printf("Table #%d :\n", i);
            afficher_table_quantif(meta->tables_quantif[i]);
            printf("\n");
        }
    }

    // Tables Huffman
    printf("TABLES HUFFMAN DC");
    for (int i = 0; i < 4; i++) {
        if (meta->table_huffman_DC[i]) {
            printf("Table DC #%d :\n", i);
            afficher_table_huffman(meta->table_huffman_DC[i]);
            printf("\n");
        }
    }

    printf("TABLES HUFFMAN AC");
    for (int i = 0; i < 4; i++) {
        if (meta->table_huffman_AC[i]) {
            printf("Table AC #%d :\n", i);
            afficher_table_huffman(meta->table_huffman_AC[i]);
            printf("\n");
        }
    }
}
