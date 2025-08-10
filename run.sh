#!/bin/bash
gcc -g main.c gameboy.c -lSDL3 && ./a.out load.gb -d | head -n 305 > output ; clear ; cat output
