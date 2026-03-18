#!/bin/bash

clear

if [ "$1" = "--release" ] || [ "$1" = "-r" ]; then
	gcc -o emu platform/raylib_gameboy.c -s -O2 \
			-lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
			-std=c11 -Wall - DNDEBUG && doxygen && clear \
			&& ./emu
else
	gcc -o emu platform/raylib_gameboy.c -g -O2 \
			-lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
			-std=c11 -Wall && doxygen && clear && ./emu
fi
