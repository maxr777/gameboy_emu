#!/bin/bash

clear

# Clean up old build files
rm -f emu 
rm -f emu.o

# Compile
# -DNDEBUG for no asserts
gcc -o emu raylib_emu.c -s -O2 \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -std=c99 -Wall

# Run if compilation succeeded
if [ -f emu ]; then
    ./emu
fi
