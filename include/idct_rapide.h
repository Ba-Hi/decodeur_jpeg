#ifndef IDCT_LOEFFLER_H
#define IDCT_LOEFFLER_H

#include <stdint.h>


void loeffler_idct_1d(double** vector);
void idct_loeffler_2d(int16_t*** izz_matrix,uint8_t*** out_matrix);


#endif // IDCT_LOEFFLER_H