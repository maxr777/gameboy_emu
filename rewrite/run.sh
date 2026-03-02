#!/bin/bash

clear

gcc -o emu raylib_emu.c -s -O2 \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -std=c11 -Wall && ./emu
