#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../include/bitstream.h"

int main() {
    // Exemple de données : 0b10101000, 0b11110000
    uint8_t data[] = { 0xA8, 0xF0 };  // binaire : 10101000 11110000
    bitstream_t *stream = create_bitstream(data, sizeof(data));
    if (!stream) {
        fprintf(stderr, "Erreur d’allocation du bitstream.\n");
        return EXIT_FAILURE;
    }

    uint32_t val;

    // Lire 3 bits : 101 → 5
    read_bitstream(stream, 3, &val, false);
    printf("Bits 1 (3 bits): %u (attendu: 5)\n", val);

    // Peek 4 bits suivants (sans avancer) : 0100 → 4
    read_bitstream(stream, 4, &val, true);
    printf("Bits 2 (peek 4 bits): %u (attendu: 4)\n", val);

    // Relire ces 4 bits (sans peek) : 0100 → 4
    read_bitstream(stream, 4, &val, false);
    printf("Bits 3 (4 bits): %u (attendu: 4)\n", val);

    // Lire 5 bits : 01111 → 15
    read_bitstream(stream, 5, &val, false);
    printf("Bits 4 (5 bits): %u (attendu: 15)\n", val);

    // Lire encore 2 bits (devrait être 00)
    read_bitstream(stream, 2, &val, false);
    printf("Bits 5 (2 bits): %u (attendu: 0)\n", val);

    free_bitstream(stream);
    return EXIT_SUCCESS;
}
