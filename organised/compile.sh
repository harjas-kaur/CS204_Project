#!/bin/bash

# Compile labels.c, error.c, and data.c
gcc labels.c error.c data.c

# Execute the compiled program a.out
./a.out

# Compile the compiler program using main.c, text.c, and data.c
gcc -o compile main.c text.c 

# Execute the compiler program on test.asm
./compile test.asm
