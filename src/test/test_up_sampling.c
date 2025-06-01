#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


float **up_C(float **bloc_c, int h_y, int v_y, int h_c, int v_c);


// comparaison flottante
static const float EPSILON = 1e-6f;
int float_equals(float a, float b) {
    return fabsf(a - b) < EPSILON;
}

// Test uniquement horizontal
void test_horizontal_only() {
    printf("Test: horizontal seul\n");
    int h_y = 2, v_y = 1, h_c = 1, v_c = 1;
    int larg_c = h_c*8, haut_c = v_c*8;
    int larg_y = h_y*8, haut_y = v_y*8;

    // Création du bloc d'entrée 8×8 avec valeur i*10+j
    float **bloc_c = malloc(haut_c*sizeof(float*));
    for (int i = 0; i < haut_c; i++) {
        bloc_c[i] = malloc(larg_c*sizeof(float));
        for (int j = 0; j < larg_c; j++) bloc_c[i][j] = i*10 + j;
    }

    
    float **up = up_C(bloc_c, h_y, v_y, h_c, v_c);
    assert(up);

    // Chaque pixel DOIT être dupliqué horizontalement
    for (int i = 0; i < haut_y; i++) {
        for (int j = 0; j < larg_y; j++) {
            int src_j = j/2;
            float expected = bloc_c[i][src_j];
            assert(float_equals(up[i][j], expected));
        }
    }

    // freeee
    for (int i = 0; i < haut_c; i++) free(bloc_c[i]);
    free(bloc_c);
    for (int i = 0; i < haut_y; i++) free(up[i]);
    free(up);
    printf(" horizontal alll good\n");
}

// Test uniquement vertical
void test_vertical_only() {
    printf("Test: vertical seulement\n");
    int h_y = 1, v_y = 2, h_c = 1, v_c = 1;
    int larg_c = h_c*8, haut_c = v_c*8;
    int larg_y = h_y*8, haut_y = v_y*8;

    float **bloc_c = malloc(haut_c*sizeof(float*));
    for (int i = 0; i < haut_c; i++) {
        bloc_c[i] = malloc(larg_c*sizeof(float));
        for (int j = 0; j < larg_c; j++) bloc_c[i][j] = i*10 + j;
    }

    float **up = up_C(bloc_c, h_y, v_y, h_c, v_c);
    assert(up);

    // Chaque pixel DOIT être dupliqué verticalement
    for (int i = 0; i < haut_y; i++) {
        for (int j = 0; j < larg_y; j++) {
            int src_i = i/2;
            float expected = bloc_c[src_i][j];
            assert(float_equals(up[i][j], expected));
        }
    }

    for (int i = 0; i < haut_c; i++) free(bloc_c[i]);
    free(bloc_c);
    for (int i = 0; i < haut_y; i++) free(up[i]);
    free(up);
    printf("vertical seuul\n");
}

// Test h et v
void test_both_directions() {
    printf("Test: vertical ET horizontal \n");
    int h_y = 2, v_y = 2, h_c = 1, v_c = 1;
    int larg_c = h_c*8, haut_c = v_c*8;
    int larg_y = h_y*8, haut_y = v_y*8;

    float **bloc_c = malloc(haut_c*sizeof(float*));
    for (int i = 0; i < haut_c; i++) {
        bloc_c[i] = malloc(larg_c*sizeof(float));
        for (int j = 0; j < larg_c; j++) bloc_c[i][j] = i*10 + j;
    }

    float **up = up_C(bloc_c, h_y, v_y, h_c, v_c);
    assert(up);

    // Pixels dupliqués horizontalement ET verticalement
    for (int i = 0; i < haut_y; i++) {
        for (int j = 0; j < larg_y; j++) {
            int src_i = i/2, src_j = j/2;
            float expected = bloc_c[src_i][src_j];
            assert(float_equals(up[i][j], expected));
        }
    }

    for (int i = 0; i < haut_c; i++) free(bloc_c[i]);
    free(bloc_c);
    for (int i = 0; i < haut_y; i++) free(up[i]);
    free(up);
    printf("les deux (h et v) sont bons\n");
}

int main() {
    printf("===Tests up_C ===\n");
    test_horizontal_only();
    test_vertical_only();
    test_both_directions();
    printf("===L'échantillonage fonctionne!===\n");
    return 0;
}
