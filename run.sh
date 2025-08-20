#!/bin/bash
gcc -g main.c gameboy.c -lSDL3 && ./a.out cpu_instrs.gb -d > output 2> errors
