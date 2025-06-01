#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../include/extraire_bitstream.h"
/*Une structure pour représenter une table de Huffman dans un fichier JPEG*/

/*Initialisation des pointeurs, pourque les initialisations ne soient pas dispersées*/
metadonnees_jpeg_t* init_metadonnes(){
    metadonnees_jpeg_t* meta = malloc(sizeof(metadonnees_jpeg_t));
    if (meta == NULL){
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return NULL;
    } 
    memset(meta, 0, sizeof(metadonnees_jpeg_t)); /*Met tous les octets à 0, et initialise tous les pointeurs à NULL*/ 
    meta->table_huffman_AC = malloc(4*sizeof(table_huffman_t*)); // 4 tables de Huffman AC
    meta->table_huffman_DC = malloc(4*sizeof(table_huffman_t*)); // same for DC
    meta->restart_interval = 0; // what is the purpose of this ?

    /* Allocation for tables_quantif is not needed as it is statically allocated */

    /*On initialise tous les pointeurs à 0, pour éviter les erreurs*/
    for (int i=0; i<4; i++){
        meta->table_huffman_AC[i] = NULL;
        meta->table_huffman_DC[i] = NULL;
        meta->tables_quantif[i] = NULL;
    }

    return meta;
}

/*Pour la lecture des octets*/
uint8_t lire_u8(FILE *image){
    return fgetc(image);
}

uint16_t lire_u16_big(FILE *image){
    uint8_t octet_haut = fgetc(image);
    uint8_t octet_bas  = fgetc(image);
    return (octet_haut << 8) | octet_bas;
}

/*Pour ignorer les sections qui ne nous interéssent pas pour le décodage*/
void ignore_segment(FILE *image){
    uint16_t taille_section = lire_u16_big(image); // taille de la section à ignorer
    fseek(image, taille_section - 2, SEEK_CUR);    
}

/*Fonctions auxiliaires*/

void traiter_sof0(FILE *image, metadonnees_jpeg_t *meta){
    /*On extrait la taille de l'image, le nombre de composantes et pour
    chaque composante don id, son facteur d'échantillonnage et l'indice
    de table de quantification*/

    lire_u16_big(image);
    lire_u8(image); /*On ignore la précisison*/
    // on peut remplacer par fseek(image, 3, SEEK_CUR); (3 octets à ignorer)

    uint16_t hauteur = lire_u16_big(image);
    uint16_t largeur = lire_u16_big(image);

    meta->hauteur = hauteur;
    meta->largeur = largeur;

    uint8_t nombre_composantes = lire_u8(image);
    /*On vérifie que le nombre de composantes ne dépasse pas 3*/
    if (nombre_composantes > 3) {
        fprintf(stderr, "Erreur : nombre de composantes trop élevé (%d)\n", nombre_composantes);
        return;
    }


    for (int i = 0; i < nombre_composantes; i++) {
        uint8_t id_composante = lire_u8(image);
        uint8_t echantillonnage_brut = lire_u8(image);  
        uint8_t h = echantillonnage_brut >> 4;       
        uint8_t v = echantillonnage_brut & 0x0F; // pour récupérer les deux derniers bits
        uint8_t indice_quantif = lire_u8(image); 

        // on stocke Y puis Cb puis Cr
        switch (id_composante) {
            case 1:
                meta->indice_quantif_Y = indice_quantif;
                meta->echantill_horizontal_Y = h;
                meta->echantill_vertical_Y = v;
                break;
            case 2:
                meta->indice_quantif_Cb = indice_quantif;
                meta->echantill_horizontal_Cb = h;
                meta->echantill_vertical_Cb = v;
                break;
            case 3:
                meta->indice_quantif_Cr = indice_quantif;
                meta->echantill_horizontal_Cr = h;
                meta->echantill_vertical_Cr = v;
                break;
            default:
                fprintf(stderr, "Composante inconnue : %d\n", id_composante);
                break;            
            }
        
        }
}

void traiter_dqt(FILE *image, metadonnees_jpeg_t *meta){
    uint16_t longueur = lire_u16_big(image); /*Pour lire la longueur totale du segment*/
    int octets_lus = 2;
    
    /*Boucle tant qu'il reste des données dans le segment DQT*/
    while (octets_lus < longueur){
        uint8_t pqtq = lire_u8(image); /*1 octet contient pq et tq*/
        octets_lus++;

        uint8_t pq = pqtq >> 4;
        uint8_t tq = pqtq & 0x0F; /*Pour l'indice de la table*/

        /*Test de validité de tq*/
        if (tq >= 4) {
            fprintf(stderr, "Indice de table de quantification invalide : %d\n", tq);
            return;
        }
        
        /*On alloue un tableau de 64 coeff sur 16 bits*/
        uint16_t* table = malloc(64* sizeof(uint16_t));
        if (table == NULL ){
            fprintf(stderr, "Échec de l'allocation mémoire pour la table de quantification\n");
            return;
        }

        if (pq == 0){
            for (int i = 0; i < 64; i++){
                table[i] = lire_u8(image);
            }
            octets_lus += 64;
        } else if (pq == 1){
            for (int i = 0; i < 64; i++){
                table[i] = lire_u16_big(image);
            }
            octets_lus += 128;
        } else {
            fprintf(stderr, "Précision non supportée : %d\n", pq);
            free(table);
            return;
        }

        meta->tables_quantif[tq] = table;
        meta->precisions_quantif[tq] = pq;
        meta-> nb_tables_quantif += 1;
        }

    }

void traiter_dht(FILE *image, metadonnees_jpeg_t *meta){
    uint16_t longueur = lire_u16_big(image);
    int octets_lus = 2;
    
    while (octets_lus < longueur) {
        uint8_t tcth = lire_u8(image);
        octets_lus++;
    
        uint8_t tc = tcth >> 4;
        uint8_t th = tcth & 0x0F;
    
        if (th >= 4){
            fprintf(stderr, "Indice de table de Huffman invalide : %d\n", th);
            return;
        }
    
        uint8_t Li[16];
        int total_codes = 0; /*Même chose que nb_symboles*/
        for (int i = 0; i < 16; i++){
            Li[i] = lire_u8(image);
            total_codes += Li[i];
        }
        octets_lus += 16;
    
        uint8_t* symboles = malloc(total_codes * sizeof(uint8_t));
        if (symboles == NULL) {
            fprintf(stderr, "Échec de l'allocation mémoire pour les symboles Huffman.\n");
            return;
        }
    
        for (int i = 0; i < total_codes; i++){
            symboles[i] = lire_u8(image);
        }
        octets_lus += total_codes;
    
        table_huffman_t* table = malloc(sizeof(table_huffman_t));
        if (table == NULL){
            fprintf(stderr, "Erreur d'allocation de la table de Huffman.\n");
            free(symboles);
            return;
        }
    
        for (int i = 0; i < 16; i++){
            table->Li[i] = Li[i];
        }
        table->symboles = symboles;
    
        if (tc == 0) {
            meta->table_huffman_DC[th] = table;
            meta->nb_tables_huffman_DC += 1;
        } else if (tc == 1) {
            meta->table_huffman_AC[th] = table;
            meta->nb_tables_huffman_AC += 1;
        } else {
            fprintf(stderr, "Type de table Huffman invalide : %d\n", tc);
            free(symboles);
            free(table);
            return;
            }
        }
    }
    

void traiter_sos(FILE *image, metadonnees_jpeg_t *meta){ // SOS : Start of Scan
    lire_u16_big(image);
    int octets_lus = 2;

    uint8_t nb_composantes = lire_u8(image);
    meta->nb_composantes_scan = nb_composantes;

    octets_lus++;

    for (int i = 0; i < nb_composantes; i++){
        uint8_t id_composante = lire_u8(image);
        uint8_t tdta = lire_u8(image);
        uint8_t td = tdta >> 4;
        uint8_t ta = tdta & 0x0F;

        octets_lus += 2;

        switch (id_composante)
        {
        case 1:
            meta->indice_huffman_Y_DC = td;
            meta->indice_huffman_Y_AC = ta;
            meta->scan_0 = id_composante;
            break;
        case 2:
            meta->indice_huffman_Cb_DC = td;
            meta->indice_huffman_Cb_AC = ta;
            meta->scan_1 = id_composante;
            break;
        case 3:
            meta->indice_huffman_Cr_DC = td;
            meta->indice_huffman_Cr_AC = ta;
            meta->scan_2 = id_composante;
            break;
        default:
            fprintf(stderr, "ID de composante inconnue dans SOS : %d\n", id_composante);
        }
    }
    lire_u8(image); /*Spectral selection start (Ss)*/
    lire_u8(image); /*Spectral selection end (Se)*/
    lire_u8(image); /*Approximation (Ah/Al)*/

    // stocker la position du flux de données
    meta->position_data = ftell(image);
    
    octets_lus += 3;
}

void traiter_dri(FILE *image, metadonnees_jpeg_t *meta) { // DRI : Define Restart Interval
    uint16_t longueur = lire_u16_big(image); 
    if (longueur != 4) { // 2 octets pour la long, 2 octets pour l'intervalle
        fprintf(stderr, "DRI: longueur inattendue %u\n", longueur);
    }
    meta->restart_interval = lire_u16_big(image);
    // il n’y a rien d’autre dans ce segment, on a lu 2 octets de longueur + 2 d’intervalle.
}


/*Fonction Principale*/
metadonnees_jpeg_t* extraire_bitstream(FILE *image){
    if (image == NULL) {
        fprintf(stderr, "Fichier invalide (image = NULL).\n");
        return NULL;
    }

    if (lire_u8(image) != 0xFF || lire_u8(image) != 0xD8) { // SOI : Start of Image
        fprintf(stderr, "Ce n'est pas un fichier JPEG valide (SOI manquant).\n");
        return NULL;
    }

    metadonnees_jpeg_t *meta = init_metadonnes();
    if (meta == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        return NULL;
    }

    while (1) {
        uint8_t prefixe = lire_u8(image);
        if (prefixe != 0xFF) continue;

        uint8_t marqueur = lire_u8(image);
        
        switch (marqueur) {
            case 0xC0: /*SOF0*/
                traiter_sof0(image, meta);
                break;
            case 0xDB: /*DQT*/
                traiter_dqt(image, meta);
                break;
            case 0xC4: /*DHT*/
                traiter_dht(image, meta);
                break;
            case 0xDA: /*SOS*/
                traiter_sos(image, meta);
                return meta;
            case 0xD9: /*EOI (normalement pas encore)*/
                fprintf(stderr, "Fin d’image atteinte sans trouver SOS.\n");
                return NULL;
            case 0xDD: /* DRI */       
                traiter_dri(image, meta);     
                break;
            default:
                ignore_segment(image);
                break;
        }
    }
    
    return meta;
}

