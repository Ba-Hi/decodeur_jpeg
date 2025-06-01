#!/bin/bash

IMAGES_DIR="images"
EXEC="./jpeg2ppm"

for image in "$IMAGES_DIR"/*jpeg "$IMAGES_DIR"/*jpg; do
    echo "=== ./jpeg2ppm sur $image ==="
    ./jpeg2ppm "$image"
    echo ""
done