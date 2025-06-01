CC = gcc
LD = gcc

# Flags de compilation
CFLAGS = -Wall -Wextra -std=c99 -I../../include -O0 -g
LDFLAGS = -lm

# Répertoires
SRC_DIR = ..
OBJ_DIR = ../../obj
TEST_DIR = .

# Fichiers source des tests
TEST_SRC = $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/test_%.c,$(TEST_DIR)/bin/test_%,$(TEST_SRC))

# Fichiers source du projet (excluant jpeg2ppm.c qui contient le main)
SRC_FILES = $(filter-out $(SRC_DIR)/jpeg2ppm.c $(SRC_DIR)/timer.c $(SRC_DIR)/jpeg2ppm_couleur.c,$(wildcard $(SRC_DIR)/*.c))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Règle par défaut
all: directories $(TEST_BINS)

# Création des répertoires nécessaires
directories:
	mkdir -p $(TEST_DIR)/bin

# Règle pour compiler les tests
$(TEST_DIR)/bin/test_%: $(TEST_DIR)/test_%.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $< $(OBJ_FILES) $(LDFLAGS) -o $@

# Règle pour compiler les fichiers objets du projet
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Règle pour exécuter tous les tests
# Règle pour exécuter tous les tests intelligemment
run: all
	@for test in $(TEST_BINS); do \
		echo "Exécution de $$test"; \
		case "$$(basename $$test)" in \
			test_extraire_bitstream|test_extraire_bloc) \
				$$test ../../images/invader.jpeg ;; \
			*) \
				$$test ;; \
		esac; \
		echo ""; \
	done


# Règle pour exécuter un test spécifique

run-%: $(TEST_DIR)/bin/test_%
	@echo "Exécution de $<"
	@$<

# Règle pour exécuter le test extraire_bitstream
run-extraire_bitstream: $(TEST_DIR)/bin/test_extraire_bitstream
	@echo "Exécution de bin/test_extraire_bitstream"
	@$(TEST_DIR)/bin/test_extraire_bitstream ../../images/invader.jpeg

# Assurez-vous que le binaire est compilé
$(TEST_DIR)/bin/test_extraire_bitstream: $(TEST_DIR)/test_extraire_bitstream.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $< $(OBJ_FILES) $(LDFLAGS) -o $@

# same pour extraire_bloc
run-extraire_bloc: $(TEST_DIR)/bin/test_extraire_bloc
	@echo "Exécution de bin/test_extraire_bloc"
	@$(TEST_DIR)/bin/test_extraire_bloc ../../images/invader.jpeg

$(TEST_DIR)/bin/test_extraire_bloc: $(TEST_DIR)/test_extraire_bloc.c $(OBJ_FILES)
	$(CC) $(CFLAGS) $< $(OBJ_FILES) $(LDFLAGS) -o $@

# Nettoyage
clean:
	rm -rf $(TEST_DIR)/bin
	rm -f test_*.ppm

.PHONY: all directories run clean