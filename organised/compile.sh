#!/bin/bash

# Make a copy of test.asm called temp.asm
cp test.asm temp.asm || { echo "Error: Unable to create temp.asm"; exit 1; }
sleep 1

# Compile labels.c, error.c, and data.c with flag to ignore warnings
gcc -w labels.c error.c data.c || { echo "Error: Compilation failed"; exit 1; }

# Execute the compiled program a.out
./a.out || { echo "Error: Execution of a.out failed"; exit 1; }

# Compile the compiler program using main.c, text.c, and data.c with flag to ignore warnings
gcc -w -o compile main.c text.c || { echo "Error: Compilation failed"; exit 1; }

# Execute the compiler program on temp.asm
./compile temp.asm || { echo "Error: Execution of compile failed"; exit 1; }

# Restore the original contents of test.asm
cp temp.asm test.asm || { echo "Error: Unable to restore original contents"; exit 1; }

# Clean up temporary files
rm temp.asm

echo "Compilation and restoration complete."
