#!/bin/bash
gcc -g main.c gameboy.c -lSDL3 && ./a.out tetris.gb -d > output 2> errors
