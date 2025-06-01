#ifndef QUANTIFICATION_INVERSE_H
#define QUANTIFICATION_INVERSE_H

#include <stdint.h>

/* Applique la quantification inverse à un vecteur de coefficients */
void quantification_inverse(float vecteur[64], uint16_t* quant_table);

#endif // QUANTIFICATION_INVERSE_H
