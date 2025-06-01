#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/reassembler_mcus.h"
#include "../include/extraire_bloc.h"
#include "../include/extraire_bitstream.h"

float **reconstituer_bloc_complet(float **blocks, int h, int v) {
    int block_size = 8; // Assuming each block is 8x8
    int full_width = h * block_size;
    int full_height = v * block_size;

    // Allocate memory for the complete block
    float **complete_block = malloc(full_height * sizeof(float *));
    for (int i = 0; i < full_height; i++) {
        complete_block[i] = malloc(full_width * sizeof(float));
    }

    // Fill the complete block with data from the smaller blocks
    for (int by = 0; by < v; by++) {
        for (int bx = 0; bx < h; bx++) {
            float *block = blocks[bx + by * h];
            for (int y = 0; y < block_size; y++) {
                for (int x = 0; x < block_size; x++) {
                    complete_block[by * block_size + y][bx * block_size + x] = block[y * block_size + x];
                }
            }
        }
    }

    return complete_block;
}