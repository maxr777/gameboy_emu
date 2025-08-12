#!/bin/bash
gcc -g main.c gameboy.c -lSDL3 && ./a.out load.gb -d &> output && clear && cat output
