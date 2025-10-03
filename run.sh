#!/bin/bash
RED='\033[31m'
GREEN='\033[32m'
RESET='\033[0m'

gcc -g main.c gameboy.c -lSDL3

if [ $? -eq 0 ]; then
	./a.out reg_f.gb -c 3000 -d > output
	last_line=$(tail -n 1 output)

	if [[ "$last_line" == *"4242"* ]]; then
		echo -e "${RED}mreg_f.gb test failed - last line contains 4242${RESET}"
	else
		echo -e "${GREEN}mreg_f.gb test passed${RESET}";
	fi

else
	echo "Compilation failed."
fi
