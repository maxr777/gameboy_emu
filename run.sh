#!/bin/bash
RED='\033[31m'
GREEN='\033[32m'
RESET='\033[0m'

gcc -g main.c gameboy.c -lSDL3

if [ $? -eq 0 ]; then
	# acceptance/bits tests
	./a.out mooneye-20240926/acceptance/bits/mem_oam.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mem_oam.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mem_oam.gb"
	else
		echo -e "${RED}FAIL${RESET} mem_oam.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/bits/reg_f.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} reg_f.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} reg_f.gb"
	else
		echo -e "${RED}FAIL${RESET} reg_f.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/bits/unused_hwio-GS.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} unused_hwio-GS.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} unused_hwio-GS.gb"
	else
		echo -e "${RED}FAIL${RESET} unused_hwio-GS.gb - Wrong values"
	fi

	# acceptance/instr tests
	./a.out mooneye-20240926/acceptance/instr/daa.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} daa.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} daa.gb"
	else
		echo -e "${RED}FAIL${RESET} daa.gb - Wrong values"
	fi

	# acceptance timing tests
	./a.out mooneye-20240926/acceptance/add_sp_e_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} add_sp_e_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} add_sp_e_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} add_sp_e_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/call_cc_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} call_cc_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} call_cc_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} call_cc_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/call_cc_timing2.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} call_cc_timing2.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} call_cc_timing2.gb"
	else
		echo -e "${RED}FAIL${RESET} call_cc_timing2.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/call_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} call_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} call_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} call_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/call_timing2.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} call_timing2.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} call_timing2.gb"
	else
		echo -e "${RED}FAIL${RESET} call_timing2.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/di_timing-GS.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} di_timing-GS.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} di_timing-GS.gb"
	else
		echo -e "${RED}FAIL${RESET} di_timing-GS.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/div_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} div_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} div_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} div_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/ei_sequence.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} ei_sequence.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} ei_sequence.gb"
	else
		echo -e "${RED}FAIL${RESET} ei_sequence.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/ei_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} ei_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} ei_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} ei_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/halt_ime0_ei.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} halt_ime0_ei.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} halt_ime0_ei.gb"
	else
		echo -e "${RED}FAIL${RESET} halt_ime0_ei.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/halt_ime0_nointr_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} halt_ime0_nointr_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} halt_ime0_nointr_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} halt_ime0_nointr_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/halt_ime1_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} halt_ime1_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} halt_ime1_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} halt_ime1_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/halt_ime1_timing2-GS.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} halt_ime1_timing2-GS.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} halt_ime1_timing2-GS.gb"
	else
		echo -e "${RED}FAIL${RESET} halt_ime1_timing2-GS.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/if_ie_registers.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} if_ie_registers.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} if_ie_registers.gb"
	else
		echo -e "${RED}FAIL${RESET} if_ie_registers.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/interrupts/ie_push.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} ie_push.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} ie_push.gb"
	else
		echo -e "${RED}FAIL${RESET} ie_push.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/intr_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} intr_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} intr_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} intr_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/jp_cc_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} jp_cc_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} jp_cc_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} jp_cc_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/jp_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} jp_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} jp_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} jp_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/ld_hl_sp_e_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} ld_hl_sp_e_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} ld_hl_sp_e_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} ld_hl_sp_e_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/pop_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} pop_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} pop_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} pop_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/push_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} push_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} push_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} push_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/rapid_di_ei.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} rapid_di_ei.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} rapid_di_ei.gb"
	else
		echo -e "${RED}FAIL${RESET} rapid_di_ei.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/ret_cc_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} ret_cc_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} ret_cc_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} ret_cc_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/ret_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} ret_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} ret_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} ret_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/reti_intr_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} reti_intr_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} reti_intr_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} reti_intr_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/reti_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} reti_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} reti_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} reti_timing.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/rst_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} rst_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} rst_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} rst_timing.gb - Wrong values"
	fi

	# acceptance/oam_dma tests
	./a.out mooneye-20240926/acceptance/oam_dma/basic.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} oam_dma/basic.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} oam_dma/basic.gb"
	else
		echo -e "${RED}FAIL${RESET} oam_dma/basic.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/oam_dma/reg_read.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} oam_dma/reg_read.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} oam_dma/reg_read.gb"
	else
		echo -e "${RED}FAIL${RESET} oam_dma/reg_read.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/oam_dma/sources-GS.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} oam_dma/sources-GS.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} oam_dma/sources-GS.gb"
	else
		echo -e "${RED}FAIL${RESET} oam_dma/sources-GS.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/oam_dma_restart.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} oam_dma_restart.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} oam_dma_restart.gb"
	else
		echo -e "${RED}FAIL${RESET} oam_dma_restart.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/oam_dma_start.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} oam_dma_start.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} oam_dma_start.gb"
	else
		echo -e "${RED}FAIL${RESET} oam_dma_start.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/oam_dma_timing.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} oam_dma_timing.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} oam_dma_timing.gb"
	else
		echo -e "${RED}FAIL${RESET} oam_dma_timing.gb - Wrong values"
	fi

	# acceptance/ppu tests
	# ./a.out mooneye-20240926/acceptance/ppu/hblank_ly_scx_timing-GS.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/hblank_ly_scx_timing-GS.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/hblank_ly_scx_timing-GS.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/hblank_ly_scx_timing-GS.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/intr_1_2_timing-GS.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_1_2_timing-GS.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/intr_1_2_timing-GS.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_1_2_timing-GS.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/intr_2_0_timing.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_0_timing.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/intr_2_0_timing.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_0_timing.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/intr_2_mode0_timing.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_mode0_timing.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/intr_2_mode0_timing.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_mode0_timing.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/intr_2_mode0_timing_sprites.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_mode0_timing_sprites.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/intr_2_mode0_timing_sprites.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_mode0_timing_sprites.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/intr_2_mode3_timing.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_mode3_timing.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/intr_2_mode3_timing.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_mode3_timing.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/intr_2_oam_ok_timing.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_oam_ok_timing.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/intr_2_oam_ok_timing.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/intr_2_oam_ok_timing.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/lcdon_timing-GS.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/lcdon_timing-GS.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/lcdon_timing-GS.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/lcdon_timing-GS.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/lcdon_write_timing-GS.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/lcdon_write_timing-GS.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/lcdon_write_timing-GS.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/lcdon_write_timing-GS.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/stat_irq_blocking.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/stat_irq_blocking.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/stat_irq_blocking.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/stat_irq_blocking.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/stat_lyc_onoff.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/stat_lyc_onoff.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/stat_lyc_onoff.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/stat_lyc_onoff.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/acceptance/ppu/vblank_stat_intr-GS.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} ppu/vblank_stat_intr-GS.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} ppu/vblank_stat_intr-GS.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} ppu/vblank_stat_intr-GS.gb - Wrong values"
	# fi

	# acceptance/timer tests
	./a.out mooneye-20240926/acceptance/timer/div_write.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/div_write.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/div_write.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/div_write.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/rapid_toggle.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/rapid_toggle.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/rapid_toggle.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/rapid_toggle.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim00.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim00.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim00.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim00.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim00_div_trigger.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim00_div_trigger.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim00_div_trigger.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim00_div_trigger.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim01.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim01.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim01.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim01.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim01_div_trigger.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim01_div_trigger.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim01_div_trigger.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim01_div_trigger.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim10.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim10.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim10.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim10.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim10_div_trigger.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim10_div_trigger.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim10_div_trigger.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim10_div_trigger.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim11.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim11.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim11.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim11.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tim11_div_trigger.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tim11_div_trigger.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tim11_div_trigger.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tim11_div_trigger.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tima_reload.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tima_reload.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tima_reload.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tima_reload.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tima_write_reloading.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tima_write_reloading.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tima_write_reloading.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tima_write_reloading.gb - Wrong values"
	fi

	./a.out mooneye-20240926/acceptance/timer/tma_write_reloading.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} timer/tma_write_reloading.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} timer/tma_write_reloading.gb"
	else
		echo -e "${RED}FAIL${RESET} timer/tma_write_reloading.gb - Wrong values"
	fi

	# emulator-only/mbc1 tests
	./a.out mooneye-20240926/emulator-only/mbc1/bits_bank1.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/bits_bank1.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/bits_bank1.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/bits_bank1.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_bank2.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/bits_bank2.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/bits_bank2.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/bits_bank2.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_mode.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/bits_mode.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/bits_mode.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/bits_mode.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/bits_ramg.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/bits_ramg.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/bits_ramg.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/bits_ramg.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/multicart_rom_8Mb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/multicart_rom_8Mb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/multicart_rom_8Mb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/multicart_rom_8Mb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/ram_256kb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/ram_256kb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/ram_256kb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/ram_256kb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/ram_64kb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/ram_64kb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/ram_64kb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/ram_64kb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/rom_512kb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/rom_512kb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/rom_512kb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/rom_512kb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/rom_1Mb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/rom_1Mb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/rom_1Mb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/rom_1Mb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/rom_2Mb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/rom_2Mb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/rom_2Mb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/rom_2Mb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/rom_4Mb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/rom_4Mb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/rom_4Mb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/rom_4Mb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/rom_8Mb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/rom_8Mb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/rom_8Mb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/rom_8Mb.gb - Wrong values"
	fi

	./a.out mooneye-20240926/emulator-only/mbc1/rom_16Mb.gb -c 10000000 -d -sldbb > output 2> errors
	ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	if [[ -z "$ld_b_b_line" ]]; then
		echo -e "${RED}FAIL${RESET} mbc1/rom_16Mb.gb - LD B, B not reached"
	elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
		echo -e "${GREEN}PASS${RESET} mbc1/rom_16Mb.gb"
	else
		echo -e "${RED}FAIL${RESET} mbc1/rom_16Mb.gb - Wrong values"
	fi

	# emulator-only/mbc2 tests
	# ./a.out mooneye-20240926/emulator-only/mbc2/bits_ramg.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc2/bits_ramg.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc2/bits_ramg.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc2/bits_ramg.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc2/bits_romb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc2/bits_romb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc2/bits_romb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc2/bits_romb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc2/bits_unused.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc2/bits_unused.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc2/bits_unused.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc2/bits_unused.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc2/ram.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc2/ram.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc2/ram.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc2/ram.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc2/rom_512kb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc2/rom_512kb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc2/rom_512kb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc2/rom_512kb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc2/rom_1Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc2/rom_1Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc2/rom_1Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc2/rom_1Mb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc2/rom_2Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc2/rom_2Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc2/rom_2Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc2/rom_2Mb.gb - Wrong values"
	# fi

	# emulator-only/mbc5 tests
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_512kb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_512kb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_512kb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_512kb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_1Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_1Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_1Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_1Mb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_2Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_2Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_2Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_2Mb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_4Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_4Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_4Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_4Mb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_8Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_8Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_8Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_8Mb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_16Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_16Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_16Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_16Mb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_32Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_32Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_32Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_32Mb.gb - Wrong values"
	# fi
	#
	# ./a.out mooneye-20240926/emulator-only/mbc5/rom_64Mb.gb -c 10000000 -d -sldbb > output 2> errors
	# ld_b_b_line=$(grep "Opcode: 0x40" output | tail -n 1)
	# if [[ -z "$ld_b_b_line" ]]; then
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_64Mb.gb - LD B, B not reached"
	# elif [[ "$ld_b_b_line" == *"BC: 0305"* ]] && [[ "$ld_b_b_line" == *"DE: 080D"* ]] && [[ "$ld_b_b_line" == *"HL: 1522"* ]]; then
	# 	echo -e "${GREEN}PASS${RESET} mbc5/rom_64Mb.gb"
	# else
	# 	echo -e "${RED}FAIL${RESET} mbc5/rom_64Mb.gb - Wrong values"
	# fi

else
	echo "Compilation failed."
fi
