#!/bin/bash
# gcc -g main.c gameboy.c -lSDL3 && ./a.out ../mooneye-20240926/acceptance/bits/reg_f.gb -c 10000000 -d -sldbb > output 2> errors
gcc -g cpu_tests.c cJSON/cJSON.c && ./a.out
