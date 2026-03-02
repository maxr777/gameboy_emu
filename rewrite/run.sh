#!/bin/bash

clear

gcc -o emu raylib_gameboy.c -s -O2 \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -std=c11 -Wall && ./emu
