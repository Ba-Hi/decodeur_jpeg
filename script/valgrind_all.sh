#!/bin/bash

IMAGES_DIR="images"
EXEC="./jpeg2ppm"

for image in "$IMAGES_DIR"/*; do
    echo "=== Valgrind sur $image ==="
    valgrind --leak-check=full --show-leak-kinds=all "$EXEC" "$image" 2>&1 | tee "valgrind_logs/$(basename "$image").log"
    echo ""
done