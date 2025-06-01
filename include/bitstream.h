#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct bitstream {
    uint8_t *data;
    size_t size;
    size_t byte_pos;
    uint8_t bit_pos;
} bitstream_t;

// Création du bitstream depuis un tableau
bitstream_t* create_bitstream(uint8_t *data, size_t size);

// Lecture de bits
int read_bitstream(bitstream_t *stream, uint8_t nbits, uint32_t *result, bool peek);

// Libération
void free_bitstream(bitstream_t *stream);


#endif
