#include "../include/bitstream.h"
#include <stdlib.h>
#include <string.h>

// Initialise le flux de bits
bitstream_t* create_bitstream(uint8_t *data, size_t size) {
    bitstream_t *stream = malloc(sizeof(bitstream_t));
    if (!stream) return NULL;

    stream->data = malloc(size);
    if (!stream->data) {
        free(stream);
        return NULL;
    }

    memcpy(stream->data, data, size);
    stream->size = size;
    stream->byte_pos = 0;
    stream->bit_pos = 0;

    return stream;
}

// Libère le flux
void free_bitstream(struct bitstream *stream) {
    if (stream) {
        free(stream->data);
        free(stream);
    }
}


// Lit nbits bits du flux
int read_bitstream(bitstream_t *stream, uint8_t nbits, uint32_t *result, bool peek) {
    
    if (!stream || !result || nbits > 32) return -1;

    size_t byte_pos_backup = stream->byte_pos;
    uint8_t bit_pos_backup = stream->bit_pos;

    uint32_t val = 0;
    for (uint8_t i = 0; i < nbits; i++) {
        if (stream->byte_pos >= stream->size) return -1;

        uint8_t byte = stream->data[stream->byte_pos];
        uint8_t bit = (byte >> (7 - stream->bit_pos)) & 1;
        val = (val << 1) | bit;

        stream->bit_pos++;
        if (stream->bit_pos >= 8) {
            stream->bit_pos = 0;
            stream->byte_pos++;
        }
    }

    if (peek) {
        stream->byte_pos = byte_pos_backup;
        stream->bit_pos = bit_pos_backup;
    }

    *result = val;
    return nbits;
}


/* Pour gérer le restart */
// void bitstream_align_to_next_byte(struct bitstream *stream) {
//     if (stream->bit_pos != 0) {
//         stream->byte_pos++;
//         stream->bit_pos = 0;
//     }
// }

// void bitstream_skip_restart_marker(bitstream_t *bs) {
//     // Forcer à se réaligner sur l'octet suivant
//     if (bs->bit_pos != 0) {
//         bs->byte_pos++;
//         bs->bit_pos = 0;
//     }

//     // Cherche et saute un éventuel marqueur FFD0–FFD7
//     while (bs->byte_pos + 1 < bs->size){
//         if (bs->data[bs->byte_pos] == 0xFF &&
//            (bs->data[bs->byte_pos + 1] >= 0xF8) == 0xD0 ){
//             bs->byte_pos += 2;
//             printf("Restart Marker");
//             return;
//            }
//         if (bs -> data[bs->byte_pos]==0xFF){
//             bs -> byte_pos++;
//         }
//         else{
//             break;
//            }
//     }
//     printf("Nope restart");
// }

