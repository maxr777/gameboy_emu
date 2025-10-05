#!/bin/bash
RED='\033[31m'
GREEN='\033[32m'
RESET='\033[0m'

gcc -g main.c gameboy.c -lSDL3

if [ $? -eq 0 ]; then
	./a.out mooneye-20240926/acceptance/bits/unused_hwio-GS.gb -c 10000000 -d > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}unused_hwio-GS.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}unused_hwio-GS.gb test failed - Fibonacci sequence not found${RESET}"
	fi

	./a.out mooneye-20240926/acceptance/bits/reg_f.gb -c 10000000 -d > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}reg_f.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}reg_f.gb test failed - Fibonacci sequence not found${RESET}"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_ramg.gb -c 10000000 -d > output 2> errors
	# Check for Fibonacci sequence in registers: B=03, C=05, D=08, E=0D, H=15, L=22
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}bits_ramg.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}bits_ramg.gb test failed - Fibonacci sequence not found${RESET}"
	fi

else
	echo "Compilation failed."
fi
