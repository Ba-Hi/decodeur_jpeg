# Décodeur JPEG - Team 11

Ce projet implémente un décodeur JPEG complet en C, capable de traiter des images en niveaux de gris et en couleur, avec ou sans sous-échantillonnage. Le décodeur suit les étapes standard du processus de décompression JPEG et produit des images au format PPM.

## Table des matières
1. [Architecture du projet](#1-architecture-du-projet)
2. [Structures de données](#2-structures-de-données)
3. [Flux de données](#3-flux-de-données)
4. [Modules et fonctionnalités](#4-modules-et-fonctionnalités)
5. [Tests unitaires](#5-tests-unitaires)
6. [Compilation et exécution](#6-compilation-et-exécution)
7. [Planning et versions](#7-planning-et-versions)
8. [Diagramme de Grantt](#8-diagramme-de-grantt)
9. [Compilation et execution](#9-compilation-et-exécution)

## 1. Architecture du projet

Le projet est organisé selon une architecture modulaire où chaque étape du décodage JPEG est implémentée dans un fichier séparé. Cette approche facilite le développement, les tests et la maintenance.

### Structure des répertoires

```
team11/
├── bin/                  # Exécutables compilés
├── images/               # Images de test et résultats
├── include/              # Fichiers d'en-tête (.h)
├── obj/                  # Fichiers objets compilés (.o)
├── script/               # Lancer jpeg2ppm et valgrind (.sh)
├── src/                  # Code source (.c)
│   └── test/             # Tests unitaires
├── jpeg              # Configuration de compilation├── Makefile              # Configuration de compilation
└── README.md             # Documentation du projet
```

### Flux de traitement

Le décodeur JPEG suit les étapes suivantes :

1. **Lecture du fichier JPEG** et extraction des métadonnées
2. **Décodage Huffman** des données compressées
3. **Quantification inverse** des coefficients DCT
4. **Réorganisation en zigzag inverse** des coefficients
5. **Transformation DCT inverse** pour obtenir les valeurs spatiales
6. **Sur-échantillonnage** des composantes de chrominance (si nécessaire)
7. **Conversion de l'espace colorimétrique** YCbCr vers RGB
8. **Écriture de l'image** au format PPM

## 2. Structures de données

Le projet utilise plusieurs structures de données clés pour représenter les différentes étapes du décodage JPEG :

### Métadonnées JPEG
```c
typedef struct {
    uint16_t largeur;                  // Largeur de l'image
    uint16_t hauteur;                  // Hauteur de l'image
    
    uint8_t nb_tables_quantif;         // Nombre de tables de quantification
    uint16_t* tables_quantif[4];       // Tables de quantification (max 4)
    uint8_t precisions_quantif[4];     // Précision de chaque table
    
    uint8_t indice_quantif_Y;          // Indice de la table pour Y
    uint8_t indice_quantif_Cb;         // Indice de la table pour Cb
    uint8_t indice_quantif_Cr;         // Indice de la table pour Cr
    
    // Facteurs d'échantillonnage
    uint8_t echantill_vertical_Y;
    uint8_t echantill_horizontal_Y;
    uint8_t echantill_vertical_Cb;
    uint8_t echantill_horizontal_Cb;
    uint8_t echantill_vertical_Cr;
    uint8_t echantill_horizontal_Cr;
    
    // Indices des tables Huffman
    uint8_t indice_huffman_Y_DC;
    uint8_t indice_huffman_Y_AC;
    uint8_t indice_huffman_Cb_DC;
    uint8_t indice_huffman_Cb_AC;
    uint8_t indice_huffman_Cr_DC;
    uint8_t indice_huffman_Cr_AC;
    
    // Tables Huffman
    uint8_t nb_tables_huffman_AC;
    uint8_t nb_tables_huffman_DC;
    table_huffman_t** table_huffman_AC;
    table_huffman_t** table_huffman_DC;
    
    // Paramètres de scan
    uint8_t scan_0;
    uint8_t scan_1;
    uint8_t scan_2;
    
    struct bitstream* stream;          // Flux de bits des données compressées
    char* nom_image;                   // Nom du fichier image
    
    long position_data;                // Position des données après le segment SOS
} metadonnees_jpeg_t;
```

### Table de Huffman
```c
typedef struct table_huffman {
    uint8_t Li[16];           // Nombre de codes de longueur i+1
    uint8_t* symboles;        // Liste des symboles
    uint8_t nb_symboles;      // Nombre total de symboles
} table_huffman_t;
```

### Bitstream
```c
struct bitstream {
    uint8_t *data;            // Données brutes
    uint32_t size;            // Taille en octets
    uint32_t position;        // Position courante en bits
};
```

### Pixel RGB
```c
typedef struct {
    uint8_t r;                // Composante rouge (0-255)
    uint8_t g;                // Composante verte (0-255)
    uint8_t b;                // Composante bleue (0-255)
} Pixel;
```

### MCU
```c
typedef struct {
    float **blocs_Y;
    float **blocs_Cb; 
    float **blocs_Cr; 
} MCU_t;
```


### Schéma des structures de données

```
+------------------+     +------------------+     +------------------+
|  Fichier JPEG    |     |   Métadonnées    |     |    Bitstream     |
|------------------|     |------------------|     |------------------|
| - En-tête        |---->| - Dimensions     |     | - Données        |
| - Tables         |     | - Tables Huffman |---->| - Position       |
| - Données        |     | - Tables Quantif |     | - Taille         |
+------------------+     +------------------+     +------------------+
                                  |
                                  v
+------------------+     +------------------+     +------------------+
| Vecteur Zigzag   |     |  Bloc iDCT 8x8   |     |   Bloc YCbCr     |
|------------------|     |------------------|     |------------------|
|  64 coefficients |<----| Fréquentiel      |---->| Spatial          |
|                  |     | Coefficients     |     | Pixels           |
+------------------+     +------------------+     +------------------+
                                                         |
                                                         v
                                               +------------------+
                                               |   Image RGB      |
                                               |------------------|
                                               | - Pixels RGB     |
                                               | - Largeur        |
                                               | - Hauteur        |
                                               +------------------+
                                                         |
                                                         v
                                               +------------------+
                                               |  Fichier PPM     |
                                               |------------------|
                                               | - En-tête        |
                                               | - Données RGB    |
                                               +------------------+
```

#### Ordre de parcours Zigzag
```
 0  1  5  6 14 15 27 28    →  0  1  2  3  4  5  6  7
 2  4  7 13 16 26 29 42       8  9 10 11 12 13 14 15
 3  8 12 17 25 30 41 43      16 17 18 19 20 21 22 23
 9 11 18 24 31 40 44 53      24 25 26 27 28 29 30 31
10 19 23 32 39 45 52 54      32 33 34 35 36 37 38 39
20 22 33 38 46 51 55 60      40 41 42 43 44 45 46 47
21 34 37 47 50 56 59 61      48 49 50 51 52 53 54 55
35 36 48 49 57 58 62 63      56 57 58 59 60 61 62 63
```

## 3. Flux de données

Voici comment les données circulent à travers les différentes étapes du décodeur :

```
Fichier JPEG → Métadonnées + Bitstream → Coefficients DCT (zigzag) → 
Coefficients DCT (quantifiés) → Coefficients DCT (dequantifiés) → 
Blocs spatiaux YCbCr → Blocs RGB → Image PPM
```

### Diagramme de flux de données

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Extraction │     │   Décodage  │     │  Quantif    │     │   Zigzag    │
│     des     │────▶│   Huffman   │────▶│   inverse   │────▶│   inverse   │
│  métadonnées│     │             │     │             │     │             │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
                                                                   │
                                                                   ▼
                                                           ┌─────────────┐
                Si échantillonage                          │    IDCT     │
        ___________________________________________________│    inverse  │      
       │                                                   └─────────────┘
       │                                            ______________│       
       │                                    sinon  │                 
       │                ┌─────────────┐     ┌──────▼──────┐     ┌─────────────┐
       └───────────────▶│      Up     │     │  Conversion │     │  Écriture   │
                        │sampling     │────▶│  YCbCr→RGB  │────▶│    PPM      │
                        │             │     │             │     │             │
                        └─────────────┘     └─────────────┘     └─────────────┘
```

## 4. Modules et fonctionnalités

Le dossier `src/` contient un fichier `.c` par étape du décodage JPEG. Voici un résumé détaillé de chaque module :

| Fichier | Rôle | Entrée(s) | Sortie(s) | Fonctions principales |
|--------|------|-----------|-----------|----------------------|
| `jpeg2ppm.c` | Programme principal | Chemin du fichier JPEG | Fichier PPM | `main()` |
| `extraire_bitstream.c` | Extraction des métadonnées | Fichier JPEG | Structure `metadonnees_jpeg_t` | `extraire_bitstream()`, `traiter_sof0()`, `traiter_dqt()`, `traiter_dht()` |
| `bitstream.c` | Gestion du flux de bits | Données brutes | Structure `bitstream` | `create_bitstream()`, `read_bit()`, `read_bits()`, `free_bitstream()` |
| `huffman_decodage.c` | Décodage Huffman | Tables Huffman, bitstream | Vecteur de coefficients | `decoder_bloc()`, `decoder_dc()`, `decoder_ac()` |
| `quantification_inverse.c` | Quantification inverse | Vecteur quantifié, table de quantification | Vecteur déquantifié | `quantification_inverse()` |
| `zz_inverse.c` | Réorganisation zigzag inverse | Vecteur de 64 coefficients | Matrice 8×8 | `zigzag_inverse()` |
| `idct.c` | DCT inverse standard | Bloc 8×8 fréquentiel | Bloc 8×8 spatial | `idct()` |
| `idct_rapide.c` | DCT inverse optimisée (Loeffler) | Bloc 8×8 fréquentiel | Bloc 8×8 spatial | `idct_2d()`, `idct_1d()`, `rotation_inverse()` |
| `up_sampling.c` | Sur-échantillonnage | Blocs Cb/Cr sous-échantillonnés | Blocs Cb/Cr pleine résolution | `upsampling()` |
| `YCbCr_2_RGB.c` | Conversion colorimétrique | Matrices Y, Cb, Cr | Matrice RGB | `convert_ycbcr_2_rgb()` |
| `ecrire_ppm.c` | Écriture du fichier image | Matrice RGB | Fichier PPM/PGM | `ecrire_image_ppm()` |

### Fonctionnalités clés

1. **Décodage Huffman** : Utilise des tables de décodage optimisées pour accélérer le processus
2. **IDCT rapide** : Implémente l'algorithme de Loeffler pour une transformation DCT inverse efficace
3. **Sur-échantillonnage** : Supporte différents facteurs d'échantillonnage (4:2:0, 4:2:2, 4:4:4)
4. **Conversion colorimétrique** : Transforme précisément les valeurs YCbCr en RGB selon la norme JPEG

## 5. Tests unitaires

Le dossier `src/test/` contient des tests unitaires pour chaque module du décodeur. Ces tests permettent de vérifier le bon fonctionnement de chaque composant individuellement.

| Test | Description |
|------|-------------|
| `test_bitstream.c` | Vérifie la lecture correcte des bits du flux |
| `test_extraire_bitstream.c` | Teste l'extraction des métadonnées JPEG |
| `test_huffman_decode.c` | Vérifie le décodage Huffman des coefficients |
| `test_quantification_inverse.c` | Teste la quantification inverse |
| `test_zz_inverse.c` | Vérifie la réorganisation zigzag inverse |
| `test_idct.c` | Teste la transformation DCT inverse standard |
| `test_idct_rapide.c` | Vérifie l'implémentation rapide de l'IDCT |
| `test_up_sampling.c` | Teste le sur-échantillonnage des composantes de chrominance |
| `test_YCbCr_2_RGB.c` | Vérifie la conversion colorimétrique |
| `test_ecrire_ppm.c` | Teste l'écriture des fichiers PPM |

Pour exécuter les tests :
```bash
cd src/test
make
make run
```

## 6. Compilation et exécution

### Compilation
```bash
make
```

### Exécution
```bash
./jpeg2ppm chemin/vers/image.jpg
```

L'image décodée sera enregistrée avec le même nom mais avec l'extension `.ppm` pour les images en couleurs et `.pgm` pour les images en niveaux de gris.

## 7. Planning et versions

Le développement du projet a suivi une approche incrémentale avec plusieurs versions :

| Version | Nom de code   | Caractéristiques | Temps estimé |
| :---:   | :---   	  | :--- | :--- |
| 1 	  | Invader 	  | Décodeur d'images 8x8 en niveaux de gris | J+ 4 |
| 2 	  | Coeur      	  | Décodeur d'images 8x8 en couleur | J+ 5 |
| 3 	  | Noir et blanc | Extension à des images grises comportant plusieurs blocs | J + 6 |
| 4 	  | Couleur 	  | Extension à des images en couleur | J+8 |
| 5	  | Sous-ech 	  | Extension avec des images avec sous-échantionnage | J +10 |

### État actuel du projet

Le décodeur est pleinement fonctionnel et prend en charge :
- Images en niveaux de gris et en couleur
- Images de toutes tailles (multiples de 8 ou non)
- Différents facteurs de sous-échantillonnage
- Optimisations de performance (IDCT rapide)

---

## 8. Diagramme de Grantt

![Notre progression](https://gitlab.ensimag.fr/-/project/55696/uploads/b1a1daf50df2480a225cb1a0176da47c/Capture_d_%C3%A9cran__96_.png)



---

## 9. Compilation et Exécution

Pour compiler le programme :

```bash
make
```

Pour nettoyer les fichiers générés :

```bash
make clean
```

Pour lancer `./jpeg2ppm` sur une image initialement donnée, se positionner à la racine du projet et exécuter :

```bash
./jpeg2ppm images/nome_de_l_image.jpg
```
Par défaut, notre décodeur utilise l'algorithme de Loeffler pour la transformée DCT inverse. Pour utiliser l'algorithme standard, il faut ajouter `--classic` comme argument lors de l'appel de `./jpeg2ppm`. Par exemple :

```bash
./jpeg2ppm images/nom_de_l_image.jpg --classic
```

Pour lancer `./jpeg2ppm` sur les images données initialements :

```bash
./script/jpeg2ppm_all.sh
```

Pour lancer `valgrind` sur les images données initialement :

```bash
./script/valgrind_all.sh
```

Pour lancer `./jpeg2ppm` sur les images qu'on ajouté dans images/tests/ :

```bash
./script/jpeg2ppm_test_all.sh
```
