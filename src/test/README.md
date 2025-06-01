# Tests Unitaires pour le Décodeur JPEG

Ce répertoire contient les tests unitaires pour les différents modules du décodeur JPEG.

## Structure des Tests

Chaque module du projet a un fichier de test correspondant nommé `test_<nom_du_module>.c`. Ces tests vérifient le bon fonctionnement des fonctions du module.

## Modules Testés

- **bitstream** : Tests pour la manipulation du flux de bits
- **ecrire_ppm** : Tests pour l'écriture d'images au format PPM
- **extraire_bitstream** : Tests pour l'extraction des métadonnées JPEG
- **extraire_bloc** : Tests pour l'extraction des blocs de données
- **huffman_decode** : Tests pour le décodage Huffman
- **idct** : Tests pour la transformée en cosinus discrète inverse et algorithme de Loeffler
- **quantification_inverse** : Tests pour la quantification inverse
- **up_sampling** : Tests pour le sur-échantillonnage
- **YCbCr_2_RGB** : Tests pour la conversion de l'espace colorimétrique pour les images en couleurs
- **YCbCr_2_RGB_nb** : Tests pour la conversion de l'espace colorimétrique pour les images B&W
- **zz_inverse** : Tests pour le zigzag inverse

## Compilation et Exécution des Tests

Pour compiler tous les tests :

```bash
make
```

Pour exécuter tous les tests :

```bash
make run
```

Pour exécuter un test spécifique (par exemple, le test de bitstream) :

```bash
make run-bitstream
```

Pour nettoyer les fichiers générés :

```bash
make clean
```
