#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "../../include/extraire_bitstream.h"
#include "../../include/huffman_decode.h"
#include "../../include/bitstream.h"
#include "../../include/extraire_bloc.h"


// Test unitaire
int main() {
    // Table canonique : 13 symboles, tous en longueur 4 (Li[3] = 13)
    uint8_t Li[16] = {0};
    Li[3] = 13;

    uint8_t symboles[13] = {'H','u','f','m','a','n',' ','w','o','r','k','s','y'};

    table_huffman_decode_t* table = construire_table_depuis_Li_symboles(Li, symboles);
    assert(table != NULL);

    uint8_t data[] = {
        0b00000001, // H u
        0b00100010, // f f
        0b00110100, // m a
        0b01010110, // n ␣
        0b01111000, // w o
        0b10011010, // r k
        0b10110110, // s ␣
        0b11000100, // y a
        0b11001100  // y y
    };

    struct bitstream* stream = create_bitstream(data, sizeof(data));

    const char* attendu = "Huffman works yayy";
    char obtenu[64] = {0};

    for (int i = 0; i < 18; i++) {
        int8_t symb = decoder_valeur_huffman(table, stream);
        obtenu[i] = (char)symb;
    }

    assert(strcmp(obtenu, attendu) == 0);
    printf("Decodage passed successfuly. \n");
    printf("Phrase décodée : %s\n", obtenu);


    liberer_table_huffman(table);
    return 0;
}