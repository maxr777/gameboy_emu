#!/bin/bash
gcc -g main.c gameboy.c -lSDL3 && ./a.out load.gb | head -n 305 > output
