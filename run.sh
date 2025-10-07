#!/bin/bash
RED='\033[31m'
GREEN='\033[32m'
RESET='\033[0m'

gcc -g main.c gameboy.c -lSDL3

if [ $? -eq 0 ]; then
	./a.out mooneye-20240926/acceptance/bits/unused_hwio-GS.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}unused_hwio-GS.gb test failed - LD B, B not reached${RESET}"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}unused_hwio-GS.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}unused_hwio-GS.gb test failed - Wrong values: $ld_b_b_line${RESET}"
	fi

	./a.out mooneye-20240926/acceptance/bits/reg_f.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}reg_f.gb test failed - LD B, B not reached${RESET}"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}reg_f.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}reg_f.gb test failed - Wrong values: $ld_b_b_line${RESET}"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_ramg.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}bits_ramg.gb test failed - LD B, B not reached${RESET}"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}bits_ramg.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}bits_ramg.gb test failed - Wrong values: $ld_b_b_line${RESET}"
	fi

	./a.out mooneye-20240926/acceptance/bits/mem_oam.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}mem_oam.gb test failed - LD B, B not reached${RESET}"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}mem_oam.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}mem_oam.gb test failed - Wrong values: $ld_b_b_line${RESET}"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_bank1.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}bits_bank1.gb test failed - LD B, B not reached${RESET}"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}bits_bank1.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}bits_bank1.gb test failed - Wrong values: $ld_b_b_line${RESET}"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_bank2.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}bits_bank2.gb test failed - LD B, B not reached${RESET}"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}bits_bank2.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}bits_bank2.gb test failed - Wrong values: $ld_b_b_line${RESET}"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_mode.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)

	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}bits_mode.gb test failed - LD B, B not reached${RESET}"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}bits_mode.gb test passed - Fibonacci sequence found${RESET}"
	else
		echo -e "${RED}bits_mode.gb test failed - Wrong values: $ld_b_b_line${RESET}"
	fi

else
	echo "Compilation failed."
fi
