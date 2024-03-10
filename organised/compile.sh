#!/bin/bash

ORIGINAL_FILE="test.asm"

if [ ! -f "$ORIGINAL_FILE" ]; then
    echo "Error: $ORIGINAL_FILE does not exist"
    exit 1
fi

cp "$ORIGINAL_FILE" temporary.txt || { echo "Error: Unable to create temporary.txt"; exit 1; }
sleep 1

gcc -w labels.c error.c data.c || { echo "Error: Compilation failed"; exit 1; }

./a.out || { echo "Error: Execution of a.out failed"; exit 1; }

gcc -w -o compile main.c text.c data.c || { echo "Error: Compilation failed"; exit 1; }

./compile temporary.txt || { echo "Error: Execution of compile failed"; exit 1; }

cp temporary.txt "$ORIGINAL_FILE" || { echo "Error: Unable to restore original contents"; exit 1; }

rm temporary.txt
rm a.out compile

echo "Compilation and restoration complete. Unnecessary files removed."
