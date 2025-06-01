#include <stdio.h>
#include <math.h> 
#include <stdint.h>
#include <../include/idct.h>

#define M_PI 3.14159265358979323846
#define CLAMP(x) ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

// Mathematic equation :
// S(x,y) = 1/sqr(2) *  sum(i=0 to 7) sum(j=0 to 7) F(i,j) * C(i) * C(j) * cos((2*x+1)*i*pi/16) * cos((2*y+1)*j*pi/16)
// C(i) = 1/sqr(2) if i=0 else 1
// C(j) = 1/sqr(2) if j=0 else 1
// F(i,j) = DCT coefficient at position (i,j)
// S(x,y) = pixel value at position (x,y)


// Fonction IDCT 8x8
void idct_8x8(float input[8][8], float output[8][8]) {
    int x, y, i, j;
    double sum;

    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            sum = 0.0;
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 8; j++) {
                    double ci = (i == 0) ? 1.0 / sqrt(2.0) : 1.0;
                    double cj = (j == 0) ? 1.0 / sqrt(2.0) : 1.0;
                    sum += ci * cj * input[i][j] *
                           cos((2 * x + 1) * i * M_PI / 16.0) *
                           cos((2 * y + 1) * j * M_PI / 16.0);
                }
            }
            output[x][y] = (uint8_t) round(CLAMP(sum / 4.0 + 128.0));

        }
    }
}




