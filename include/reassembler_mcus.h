#ifndef REASSEMBLER_BLOCS_H
#define REASSEMBLER_BLOCS_H

#include <stdint.h>
#include "extraire_bloc.h"/*Nécessaire pour la structure MCU_t*/
#include "extraire_bitstream.h"   
/*Reconstitue une image à partir des blocs d’une composante ('Y', 'Cb', ou 'Cr')
et extraits MCU par MCU et les range dans l’ordre spatial.
Varibles introduites :asm
-mcus : Tableau de MCUs contenant les blocs de chaque composante
-nb_mcus_x : Nombre de MCUs par ligne
-nb_mcus_y : Nombre de MCUs par colonne
-h         : Facteur d’échantillonnage horizontal
-v         : Facteur d’échantillonnage vertical
-composante: Composante à extraire ('Y', 'Cb' ou 'Cr')
-Valeur de retour : float** Tableau de blocs 8x8 (tableau plat de pointeurs vers des blocs de 64 floats)
 */

float **reconstituer_bloc_complet(float **blocks, int h, int v);

#endif // REASSEMBLER_BLOCS_H
