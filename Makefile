CC = gcc
LD = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O0 -g
LDFLAGS = -lm

# Dossiers
BIN_DIR = bin/sh
OBJ_DIR = obj
SRC_DIR = src
IMG_DIR = images

# Fichiers source communs
SRC_COMMON = $(SRC_DIR)/bitstream.c $(SRC_DIR)/ecrire_ppm.c $(SRC_DIR)/extraire_bitstream.c \
             $(SRC_DIR)/extraire_bloc.c $(SRC_DIR)/huffman_decodage.c $(SRC_DIR)/idct.c \
             $(SRC_DIR)/idct_rapide.c $(SRC_DIR)/quantification_inverse.c \
             $(SRC_DIR)/up_sampling.c $(SRC_DIR)/YCbCr_2_RGB.c $(SRC_DIR)/zz_inverse.c \
             $(SRC_DIR)/traiter_image_couleur.c $(SRC_DIR)/traiter_image_noir_blanc.c \
             $(SRC_DIR)/YCbCr_2_RGB_nb.c $(SRC_DIR)/reassembler_mcus.c

# Objets
OBJ_COMMON = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_COMMON))
OBJ_MAIN = $(OBJ_DIR)/jpeg2ppm.o

# Cible par défaut
all: directories jpeg2ppm

# Compilation de jpeg2ppm → dans bin/, puis copie à la racine
jpeg2ppm: $(BIN_DIR)/jpeg2ppm
	@cp $(BIN_DIR)/jpeg2ppm jpeg2ppm

$(BIN_DIR)/jpeg2ppm: $(OBJ_MAIN) $(OBJ_COMMON)
	$(LD) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# Exécution avec une image par défaut
.PHONY: run
# Nettoyage complet
.PHONY: clean
clean:
	rm -r $(OBJ_DIR) $(BIN_DIR) jpeg2ppm

profile: jpeg2ppm
	./jpeg2ppm images/gris.jpg
	gprof jpeg2ppm gmon.out > result_gprof.txt
	less result_gprof.txt