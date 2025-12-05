#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "constants.h"
#include "gameboy.h"

// TODO: create defines for MBC addresses

// ================ HARDWARE ================

bool display[160][144] = {0};
uint8_t ram[8192] = {0};
uint8_t vram[8192] = {0};
uint8_t io_registers[128] = {0};
uint8_t oam[160] = {0};
uint8_t hram[127] = {0};

// ime_enable_counter: EI instruction has a 1-instruction delay before enabling interrupts
CPU cpu = {
    .regs = {0},
    .cycle = 0,
    .prefix = false,
    .ime = false,
    .ime_enable_counter = -1, // 1->0->enable ime, -1=inactive
};

Timer_controls timer_controls = {
    .div_cycle_counter = 0,
    .tima_cycle_counter = 0,
    .tac_enable = false,
    .tac_increment_cycles = 256, // 00 corresponds to increment every 256 cycles
};

// ================ ROM STUFF ================

// bootix bootrom: https://github.com/Hacktix/Bootix
ROM rom = {
    .game_rom = NULL, // initialized in main.c
    .game_size = 0,   // initialized in main.c
    .cartridge_header = {0},
    .boot_rom_enabled = true,
    .boot_rom = {
	0x31, 0xfe, 0xff, 0x21, 0xff, 0x9f, 0xaf, 0x32, 0xcb, 0x7c, 0x20, 0xfa,
	0x0e, 0x11, 0x21, 0x26, 0xff, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3,
	0x32, 0xe2, 0x0c, 0x3e, 0x77, 0x32, 0xe2, 0x11, 0x04, 0x01, 0x21, 0x10,
	0x80, 0x1a, 0xcd, 0xb8, 0x00, 0x1a, 0xcb, 0x37, 0xcd, 0xb8, 0x00, 0x13,
	0x7b, 0xfe, 0x34, 0x20, 0xf0, 0x11, 0xcc, 0x00, 0x06, 0x08, 0x1a, 0x13,
	0x22, 0x23, 0x05, 0x20, 0xf9, 0x21, 0x04, 0x99, 0x01, 0x0c, 0x01, 0xcd,
	0xb1, 0x00, 0x3e, 0x19, 0x77, 0x21, 0x24, 0x99, 0x0e, 0x0c, 0xcd, 0xb1,
	0x00, 0x3e, 0x91, 0xe0, 0x40, 0x06, 0x10, 0x11, 0xd4, 0x00, 0x78, 0xe0,
	0x43, 0x05, 0x7b, 0xfe, 0xd8, 0x28, 0x04, 0x1a, 0xe0, 0x47, 0x13, 0x0e,
	0x1c, 0xcd, 0xa7, 0x00, 0xaf, 0x90, 0xe0, 0x43, 0x05, 0x0e, 0x1c, 0xcd,
	0xa7, 0x00, 0xaf, 0xb0, 0x20, 0xe0, 0xe0, 0x43, 0x3e, 0x83, 0xcd, 0x9f,
	0x00, 0x0e, 0x27, 0xcd, 0xa7, 0x00, 0x3e, 0xc1, 0xcd, 0x9f, 0x00, 0x11,
	0x8a, 0x01, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x1b, 0x7a, 0xb3, 0x20,
	0xf5, 0x18, 0x49, 0x0e, 0x13, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xc9, 0xf0,
	0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20, 0xf7, 0xc9, 0x78, 0x22, 0x04,
	0x0d, 0x20, 0xfa, 0xc9, 0x47, 0x0e, 0x04, 0xaf, 0xc5, 0xcb, 0x10, 0x17,
	0xc1, 0xcb, 0x10, 0x17, 0x0d, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9,
	0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c, 0x00, 0x54, 0xa8, 0xfc,
	0x42, 0x4f, 0x4f, 0x54, 0x49, 0x58, 0x2e, 0x44, 0x4d, 0x47, 0x20, 0x76,
	0x31, 0x2e, 0x32, 0x00, 0x3e, 0xff, 0xc6, 0x01, 0x0b, 0x1e, 0xd8, 0x21,
	0x4d, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3e, 0x01, 0xe0, 0x50},
    .current_rom_bank = 1,
    .max_rom_banks = 0,
    .current_ram_bank = 0,
    .external_ram = {0}, // this gets set in main.c
};

MBC1_State mbc1 = {
    .ram_enable = false,
    .first_rom_bank_reg = 0,
    .second_rom_bank_reg = 0,
    .ram_bank_number = 1,
    .banking_mode_is_advanced = false,
};

void rom_write(const uint16_t addr, const uint8_t val) {
	switch (rom.cartridge_header.cartridge_type) {
	case 0x00:
		fprintf(stderr, "Writing to ROM with MCB0 is prohibited\n");
		break;
	case 0x01:
	case 0x02:
	case 0x03:
		mbc1_write(addr, val);
		break;
	default:
		fprintf(stderr, "rom_write(): Unimplemented MCB\n");
		break;
	}
}

// addresses: https://gbdev.io/pandocs/MBC1.html
void mbc1_write(const uint16_t addr, const uint8_t val) {
	if (addr < 0x2000) { // RAM enable
		if ((val & 0x0F) == 0x0A)
			mbc1.ram_enable = true;
		else
			mbc1.ram_enable = false;
	} else if (addr < 0x4000) { // ROM bank number
		mbc1.first_rom_bank_reg = val & 0x1F;
		rom.current_rom_bank = (mbc1.second_rom_bank_reg << 5) + mbc1.first_rom_bank_reg;
		rom.current_rom_bank &= rom.max_rom_banks - 1;
		if (rom.current_rom_bank == 0) rom.current_rom_bank = 1;
	} else if (addr < 0x6000) { // RAM bank number/upper bits of ROM bank number
		mbc1.second_rom_bank_reg = val & 0x03;
		rom.current_rom_bank = (mbc1.second_rom_bank_reg << 5) + mbc1.first_rom_bank_reg;
		rom.current_rom_bank &= rom.max_rom_banks - 1;
		if (rom.current_rom_bank == 0) rom.current_rom_bank = 1;
		if (mbc1.banking_mode_is_advanced) rom.current_ram_bank = val & 0x03;
	} else if (addr < 0x8000) { // banking mode select
		mbc1.banking_mode_is_advanced = val & 0x01;
	} else { // ofc vram is in between extern ram and rom banks, but that gets distinguished in write8
		if (mbc1.ram_enable)
			rom.external_ram[(addr - EXTERN_RAM_ADDR) + (rom.current_ram_bank * EXTERN_RAM_SIZE)] = val;
	}
}

uint8_t rom_read(const uint16_t addr) {
	if (rom.boot_rom_enabled && addr <= 0x00FF)
		return rom.boot_rom[addr];
	switch (rom.cartridge_header.cartridge_type) {
	case 0x00:
		return rom.game_rom[addr];
	case 0x01:
		return rom.game_rom[addr];
	case 0x02:
	case 0x03:
		return mbc1_read(addr);
	default:
		fprintf(stderr, "rom_read(): Unimplemented MCB\n");
		return 0;
	}
}

uint8_t mbc1_read(const uint16_t addr) {
	if (addr < 0x4000) { // ROM bank X0
		if (mbc1.banking_mode_is_advanced) {
			uint8_t bank = (mbc1.second_rom_bank_reg << 5) & (rom.max_rom_banks - 1);
			return rom.game_rom[addr + (ROM_BANK_SIZE * bank)];
		}
		return rom.game_rom[addr];
	} else if (addr < 0xA000) { // ROM bank 01-7F
		return rom.game_rom[(addr - ROM_BANK_N_ADDR) + (ROM_BANK_SIZE * rom.current_rom_bank)];
	} else { // RAM bank 00-03 (if any)
		if (!mbc1.ram_enable)
			return 0xFF;
		return rom.external_ram[(addr - EXTERN_RAM_ADDR) + (rom.current_ram_bank * EXTERN_RAM_SIZE)];
	}
}

// ================ HELPER FUNCTIONS ================

bool get_flag(const int flag) {
	switch (flag) {
	case Z:
		return (cpu.regs[AF].low & 0x80);
	case N:
		return (cpu.regs[AF].low & 0x40);
	case H:
		return (cpu.regs[AF].low & 0x20);
	case C:
		return (cpu.regs[AF].low & 0x10);
	default:
		fprintf(stderr, "check_flag() got an incorrect flag int (flags are 0-3 in the Flags enum)\n");
		return false;
	}
}

void set_flag(const int flag, const bool value) {
	switch (flag) {
	case Z:
		if (value)
			cpu.regs[AF].low |= 0x80;
		else
			cpu.regs[AF].low &= ~0x80;
		return;
	case N:
		if (value)
			cpu.regs[AF].low |= 0x40;
		else
			cpu.regs[AF].low &= ~0x40;
		return;
	case H:
		if (value)
			cpu.regs[AF].low |= 0x20;
		else
			cpu.regs[AF].low &= ~0x20;
		return;
	case C:
		if (value)
			cpu.regs[AF].low |= 0x10;
		else
			cpu.regs[AF].low &= ~0x10;
		return;
	default:
		fprintf(stderr, "set_flag() got an incorrect flag int (flags are 0-3 in the Flags enum)\n");
		return;
	}
}

void write16(const uint16_t addr, const uint16_t val) {
	write8(addr, val);
	write8(addr + 1, val >> 8);
}

uint16_t read16(const uint16_t addr) {
	return read8(addr) | (read8(addr + 1) << 8);
}

void write8(const uint16_t addr, const uint8_t val) {
	if (addr < ROM_BANK_N_ADDR)
		rom_write(addr, val);
	else if (addr < VRAM_ADDR)
		rom_write(addr, val);
	else if (addr < EXTERN_RAM_ADDR)
		vram[addr - VRAM_ADDR] = val;
	else if (addr < WRAM_0_ADDR)
		rom_write(addr, val);
	else if (addr < WRAM_N_ADDR)
		;
	else if (addr < ECHO_RAM_ADDR)
		ram[addr - WRAM_0_ADDR] = val;
	else if (addr < OAM_ADDR)
		fprintf(stderr, "write8: use of echo ram is prohibited\n");
	else if (addr < INVAL_MEM_ADDR)
		oam[addr - OAM_ADDR] = val;
	else if (addr < IO_REGS_ADDR)
		fprintf(stderr, "write8: use of 0xFEA0-0xFEFF is prohibited\n");
	else if (addr < HRAM_ADDR) {
		io_registers[addr - IO_REGS_ADDR] = val;
		if (addr == SERIAL_TRANSFER)
			printf("%c", val);
		else if (addr == DIV_ADDR) {
			io_registers[addr - IO_REGS_ADDR] = 0;
			timer_controls.div_cycle_counter = 0;
		} else if (addr == TAC_ADDR) {
			timer_controls.tac_enable = val & 0x04;
			switch (val & 0x03) {
			case 0x00:
				timer_controls.tac_increment_cycles = TAC_00_CYCLES;
				break;
			case 0x01:
				timer_controls.tac_increment_cycles = TAC_01_CYCLES;
				break;
			case 0x02:
				timer_controls.tac_increment_cycles = TAC_10_CYCLES;
				break;
			case 0x03:
				timer_controls.tac_increment_cycles = TAC_11_CYCLES;
				break;
			}
		}
	} else if (addr < INT_ENABLE_ADDR)
		hram[addr - HRAM_ADDR] = val;
	else {
		fprintf(stderr, "IE writes aren't implemented yet\n");
	}
}

uint8_t read8(const uint16_t addr) {
	if (addr < ROM_BANK_N_ADDR)
		return rom_read(addr);
	else if (addr < VRAM_ADDR)
		return rom_read(addr);
	else if (addr < EXTERN_RAM_ADDR)
		return vram[addr - VRAM_ADDR];
	else if (addr < WRAM_0_ADDR)
		return rom_read(addr);
	else if (addr < WRAM_N_ADDR)
		return ram[addr - WRAM_0_ADDR];
	else if (addr < ECHO_RAM_ADDR)
		return ram[addr - WRAM_0_ADDR];
	else if (addr < OAM_ADDR) {
		fprintf(stderr, "read8: use of echo ram is prohibited\n");
		return 0;
	} else if (addr < INVAL_MEM_ADDR)
		return oam[addr - OAM_ADDR];
	else if (addr < IO_REGS_ADDR) {
		fprintf(stderr, "read8: use of 0xFEA0-0xFEFF is prohibited\n");
		return 0;
	} else if (addr < HRAM_ADDR) {
		// https://github.com/Gekkio/mooneye-test-suite?tab=readme-ov-file#passfail-reporting
		// If you don't have a full Game boy system, pass/fail reporting can be sped up by making
		// sure LY (0xff44) and SC (0xff02) both return 0xff when read. This will bypass some
		// unnecessary drawing code and waiting for serial transfer to finish.
		if ((addr == SERIAL_CONTROL) || (addr == LCD_Y)) return 0xFF;
		return io_registers[addr - IO_REGS_ADDR];
	} else if (addr < INT_ENABLE_ADDR)
		return hram[addr - HRAM_ADDR];
	else
		fprintf(stderr, "IE reads aren't implemented yet\n");

	return 0;
}

// ================ OPCODES ================

// https://rgbds.gbdev.io/docs/v0.9.2/gbz80.7
// naming is instruction_destination_source
// r8 - 8 bit register, r16 - 16 bit
// HL, A, C, SP - hard set registers
// a16 == [r16], addr16 == [n16]
// aHL = [HL], aC = [C]

// The prefix instructions have one less PC increment
// than in the documentation, because I increment the PC
// when the prefix gets toggled.
// So, if the documentation says "bytes: 2," then I
// increment the PC by one, since I already incremented
// it before (at 0xCB no-prefix)

// ================ LOADS ================

void ld_r8_r8(uint8_t *dest, const uint8_t src) {
	*dest = src;

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void ld_r8_n8(uint8_t *dest, const uint8_t val) {
	*dest = val;

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void ld_r16_n16(uint16_t *dest, const uint16_t val) {
	*dest = val;

	cpu.regs[PC].full += 3;
	cpu.cycle += 3;
}

void ld_aHL_r8(const uint8_t src) {
	write8(cpu.regs[HL].full, src);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_aHL_n8(const uint8_t val) {
	write8(cpu.regs[HL].full, val);

	cpu.regs[PC].full += 2;
	cpu.cycle += 3;
}

void ld_r8_aHL(uint8_t *dest) {
	*dest = read8(cpu.regs[HL].full);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_a16_A(const uint16_t addr) {
	write8(addr, cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_addr16_A(const uint16_t addr) {
	write8(addr, cpu.regs[AF].high);

	cpu.regs[PC].full += 3;
	cpu.cycle += 4;
}

void ldh_addr16_A(const uint16_t addr) {
	write8(addr, cpu.regs[AF].high);

	cpu.regs[PC].full += 2;
	cpu.cycle += 3;
}

void ldh_aC_A() {
	write8(0xFF00 + cpu.regs[BC].low, cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_A_a16(uint16_t addr) {
	cpu.regs[AF].high = read8(addr);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_A_addr16(uint16_t addr) {
	cpu.regs[AF].high = read8(addr);

	cpu.regs[PC].full += 3;
	cpu.cycle += 4;
}

void ldh_A_addr16(const uint16_t addr) {
	cpu.regs[AF].high = read8(addr);

	cpu.regs[PC].full += 2;
	cpu.cycle += 3;
}

void ldh_A_aC() {
	cpu.regs[AF].high = read8(0xFF00 + cpu.regs[BC].low);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_aHLi_A() {
	write8(cpu.regs[HL].full, cpu.regs[AF].high);
	++cpu.regs[HL].full;

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_aHLd_A() {
	write8(cpu.regs[HL].full, cpu.regs[AF].high);
	--cpu.regs[HL].full;

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_A_aHLi() {
	cpu.regs[AF].high = read8(cpu.regs[HL].full);
	++cpu.regs[HL].full;

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void ld_A_aHLd() {
	cpu.regs[AF].high = read8(cpu.regs[HL].full);
	--cpu.regs[HL].full;

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

// ================ 8-BIT ARITHMETIC ================

void add_A_r8(const uint8_t src) {
	set_flag(N, false);
	(cpu.regs[AF].high & 0x0F) + (src & 0x0F) > 0x0F ? set_flag(H, true) : set_flag(H, false);
	(cpu.regs[AF].high + src) > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high += src;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void add_A_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);

	set_flag(N, false);
	(cpu.regs[AF].high & 0x0F) + (val & 0x0F) > 0x0F ? set_flag(H, true) : set_flag(H, false);
	(cpu.regs[AF].high + val) > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high += val;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void add_A_n8(const uint8_t val) {
	set_flag(N, false);
	(cpu.regs[AF].high & 0x0F) + (val & 0x0F) > 0x0F ? set_flag(H, true) : set_flag(H, false);
	(cpu.regs[AF].high + val) > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high += val;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void adc_A_r8(const uint8_t src) {
	set_flag(N, false);
	bool c = get_flag(C);

	(cpu.regs[AF].high & 0x0F) + (src & 0x0F) + c > 0x0F ? set_flag(H, true) : set_flag(H, false);
	(uint16_t)cpu.regs[AF].high + src + c > 0x00FF ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high += src + c;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void adc_A_aHL() {
	set_flag(N, false);
	bool c = get_flag(C);
	uint8_t val = read8(cpu.regs[HL].full);

	(cpu.regs[AF].high & 0x0F) + (val & 0x0F) + c > 0x0F ? set_flag(H, true) : set_flag(H, false);
	(uint16_t)cpu.regs[AF].high + val + c > 0x00FF ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high += val + c;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void adc_A_n8(const uint8_t val) {
	set_flag(N, false);
	bool c = get_flag(C);

	(cpu.regs[AF].high & 0x0F) + (val & 0x0F) + c > 0x0F ? set_flag(H, true) : set_flag(H, false);
	(uint16_t)cpu.regs[AF].high + val + c > 0x00FF ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high += val + c;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void sub_A_r8(const uint8_t src) {
	set_flag(N, true);

	(src & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	src > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high -= src;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void sub_A_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);

	set_flag(N, true);
	(val & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	val > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high -= val;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void sub_A_n8(const uint8_t val) {
	set_flag(N, true);
	(val & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	val > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high -= val;
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void sbc_A_r8(const uint8_t src) {
	set_flag(N, true);
	bool c = get_flag(C);

	((src + c) & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	src + c > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high -= (src + c);
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void sbc_A_aHL() {
	set_flag(N, true);
	bool c = get_flag(C);
	uint8_t val = read8(cpu.regs[HL].full);

	((val + c) & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	val + c > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high -= (val + c);
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void sbc_A_n8(const uint8_t val) {
	set_flag(N, true);
	bool c = get_flag(C);

	((val + c) & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	val + c > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[AF].high -= (val + c);
	set_flag(Z, !cpu.regs[AF].high);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void dec_r8(uint8_t *dest) {
	// lower 4 bits only borrow if they're 0000
	((*dest)-- & 0x0F) == 0x00 ? set_flag(H, true) : set_flag(H, false);

	set_flag(Z, !(*dest));
	set_flag(N, true);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void dec_aHL() {
	uint8_t result = read8(cpu.regs[HL].full);

	(result-- & 0x0F) == 0x00 ? set_flag(H, true) : set_flag(H, false);
	set_flag(Z, !result);
	set_flag(N, true);

	write8(cpu.regs[HL].full, result);

	cpu.regs[PC].full += 1;
	cpu.cycle += 3;
}

void inc_r8(uint8_t *dest) {
	((*dest)++ & 0x0F) == 0x0F ? set_flag(H, true) : set_flag(H, false);

	set_flag(Z, !(*dest));
	set_flag(N, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void inc_aHL() {
	uint8_t result = read8(cpu.regs[HL].full);

	(result++ & 0x0F) == 0x0F ? set_flag(H, true) : set_flag(H, false);
	set_flag(Z, !result);
	set_flag(N, false);

	write8(cpu.regs[HL].full, result);

	cpu.regs[PC].full += 1;
	cpu.cycle += 3;
}

void cp_A_r8(const uint8_t src) {
	uint8_t result = cpu.regs[AF].high - src;

	set_flag(Z, !result);
	set_flag(N, true);
	(src & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	src > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void cp_A_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	uint8_t result = cpu.regs[AF].high - val;

	set_flag(Z, !result);
	set_flag(N, true);
	(val & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	val > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void cp_A_n8(const uint8_t val) {
	uint8_t result = cpu.regs[AF].high - val;

	set_flag(Z, !result);
	set_flag(N, true);
	(val & 0x0F) > (cpu.regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
	val > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

// ================ 16-BIT ARITHMETIC ================

void add_HL_r16(const uint16_t src) {
	set_flag(N, false);

	(cpu.regs[HL].full + src) > cpu.regs[HL].full ? set_flag(C, true) : set_flag(C, false);
	(cpu.regs[HL].full & 0x0FFF) + (src & 0x0FFF) > 0x0FFF ? set_flag(H, true) : set_flag(H, false);

	cpu.regs[HL].full += src;

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void dec_r16(uint16_t *dest) {
	--(*dest);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void inc_r16(uint16_t *dest) {
	++(*dest);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

// ================ BITWISE LOGIC ================

void and_A_r8(const uint8_t src) {
	cpu.regs[AF].high &= src;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, true);
	set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void and_A_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	cpu.regs[AF].high &= val;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, true);
	set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void and_A_n8(const uint8_t val) {
	cpu.regs[AF].high &= val;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, true);
	set_flag(C, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void or_A_r8(const uint8_t src) {
	cpu.regs[AF].high |= src;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void or_A_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	cpu.regs[AF].high |= val;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void or_A_n8(const uint8_t val) {
	cpu.regs[AF].high |= val;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void cpl() {
	cpu.regs[AF].high = ~cpu.regs[AF].high;

	set_flag(N, true);
	set_flag(H, true);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void xor_A_r8(const uint8_t src) {
	cpu.regs[AF].high ^= src;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void xor_A_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	cpu.regs[AF].high ^= val;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void xor_A_n8(const uint8_t val) {
	cpu.regs[AF].high ^= val;

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

// ================ BIT FLAGS ================

void bit_u3_r8(const int bit_num, const uint8_t src) {
	src & (1 << bit_num) ? set_flag(Z, false) : set_flag(Z, true);

	set_flag(N, false);
	set_flag(H, true);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void bit_u3_aHL(const int bit_num) {
	read8(cpu.regs[HL].full) & (1 << bit_num) ? set_flag(Z, false) : set_flag(Z, true);

	set_flag(N, false);
	set_flag(H, true);

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void res_u3_r8(const int bit_num, uint8_t *src) {
	*src &= ~(1 << bit_num);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void res_u3_aHL(const int bit_num) {
	uint8_t val = read8(cpu.regs[HL].full);
	val &= ~(1 << bit_num);
	write8(cpu.regs[HL].full, val);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void set_u3_r8(const int bit_num, uint8_t *src) {
	*src |= (1 << bit_num);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void set_u3_aHL(const int bit_num) {
	uint8_t val = read8(cpu.regs[HL].full);
	val |= (1 << bit_num);
	write8(cpu.regs[HL].full, val);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

// ================ BIT SHIFTS ================

void rla() {
	set_flag(Z, false);
	set_flag(N, false);
	set_flag(H, false);

	bool old_carry = get_flag(C);
	bool new_carry = (cpu.regs[AF].high & 0x80) == 0x80;
	set_flag(C, new_carry);

	cpu.regs[AF].high <<= 1;

	if (old_carry)
		cpu.regs[AF].high |= 1;
	else
		cpu.regs[AF].high &= ~1;

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void rlca() {
	set_flag(Z, false);
	set_flag(N, false);
	set_flag(H, false);

	bool carry = (cpu.regs[AF].high & 0x80) == 0x80;
	set_flag(C, carry);

	cpu.regs[AF].high <<= 1;

	if (carry)
		cpu.regs[AF].high |= 1;
	else
		cpu.regs[AF].high &= ~1;

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void rra() {
	set_flag(Z, false);
	set_flag(N, false);
	set_flag(H, false);

	bool old_carry = get_flag(C);
	bool new_carry = (cpu.regs[AF].high & 0x01) == 0x01;
	set_flag(C, new_carry);

	cpu.regs[AF].high >>= 1;

	if (old_carry)
		cpu.regs[AF].high |= (1 << 7);
	else
		cpu.regs[AF].high &= ~(1 << 7);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void rrca() {
	set_flag(Z, false);
	set_flag(N, false);
	set_flag(H, false);

	bool carry = (cpu.regs[AF].high & 0x01) == 0x01;
	set_flag(C, carry);

	cpu.regs[AF].high >>= 1;
	if (carry)
		cpu.regs[AF].high |= (1 << 7);
	else
		cpu.regs[AF].high &= ~(1 << 7);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void rlc_r8(uint8_t *src) {
	bool carry = *src & 0x80;
	set_flag(C, carry);
	*src <<= 1;
	*src |= carry;
	set_flag(Z, !(*src));
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void rlc_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	bool carry = val & 0x80;
	set_flag(C, carry);
	val <<= 1;
	val |= carry;
	write8(cpu.regs[HL].full, val);

	set_flag(Z, !val);
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void rrc_r8(uint8_t *src) {
	bool carry = *src & 0x01;
	set_flag(C, carry);
	*src >>= 1;
	*src |= (carry << 7);
	set_flag(Z, !(*src));
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void rrc_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	bool carry = val & 0x01;
	set_flag(C, carry);
	val >>= 1;
	val |= (carry << 7);
	write8(cpu.regs[HL].full, val);

	set_flag(Z, !val);
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void rl_r8(uint8_t *src) {
	bool carry = *src & 0x80;
	*src <<= 1;
	*src |= get_flag(C);
	set_flag(C, carry);

	set_flag(Z, !(*src));
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void rl_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	bool carry = val & 0x80;
	val <<= 1;
	val |= get_flag(C);
	set_flag(C, carry);
	write8(cpu.regs[HL].full, val);

	set_flag(Z, !val);
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void rr_r8(uint8_t *src) {
	bool carry = *src & 0x01;
	*src >>= 1;
	*src |= (get_flag(C) << 7);
	set_flag(C, carry);

	set_flag(Z, !(*src));
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void rr_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	bool carry = val & 0x01;
	val >>= 1;
	val |= (get_flag(C) << 7);
	set_flag(C, carry);
	write8(cpu.regs[HL].full, val);

	set_flag(Z, !val);
	set_flag(N, false);
	set_flag(H, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void sla_r8(uint8_t *src) {
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, *src & 0x80);
	*src <<= 1;
	set_flag(Z, !(*src));

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void sla_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, val & 0x80);
	val <<= 1;
	write8(cpu.regs[HL].full, val);
	set_flag(Z, !val);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void sra_r8(uint8_t *src) {
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, *src & 0x01);
	bool sign_bit = *src & 0x80;
	*src >>= 1;
	set_flag(Z, !(*src));
	*src |= (sign_bit << 7);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void sra_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, val & 0x01);
	bool sign_bit = val & 0x80;
	val >>= 1;
	set_flag(Z, !val);
	val |= (sign_bit << 7);
	write8(cpu.regs[HL].full, val);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void swap_r8(uint8_t *src) {
	uint8_t high = *src & 0xF0;
	uint8_t low = *src & 0x0F;
	*src = (low << 4) | (high >> 4);

	set_flag(Z, !(*src));
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void swap_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);
	uint8_t high = val & 0xF0;
	uint8_t low = val & 0x0F;
	val = (low << 4) | (high >> 4);
	write8(cpu.regs[HL].full, val);

	set_flag(Z, !val);
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, false);

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

void srl_r8(uint8_t *src) {
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, *src & 0x01);
	*src >>= 1;
	set_flag(Z, !(*src));

	cpu.regs[PC].full += 2;
	cpu.cycle += 2;
}

void srl_aHL() {
	uint8_t val = read8(cpu.regs[HL].full);

	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, val & 0x01);
	val >>= 1;
	write8(cpu.regs[HL].full, val);
	set_flag(Z, !(val));

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

// ================ JUMPS ================

void call_n16(const uint16_t addr) {
	cpu.regs[SP].full -= 2;
	write16(cpu.regs[SP].full, cpu.regs[PC].full + 3);

	cpu.regs[PC].full = addr;

	cpu.cycle += 6;
}

void call_cc_n16(const int flag, const bool flag_state, const uint16_t addr) {
	if (get_flag(flag) == flag_state) {
		cpu.regs[SP].full -= 2;
		write16(cpu.regs[SP].full, cpu.regs[PC].full + 3);
		cpu.regs[PC].full = addr;
		cpu.cycle += 6;
	} else {
		cpu.regs[PC].full += 3;
		cpu.cycle += 3;
	}
}

void jp_n16(const uint16_t addr) {
	cpu.regs[PC].full = addr;

	cpu.cycle += 4;
}

void jp_cc_n16(const int flag, const bool flag_state, const uint16_t addr) {
	if (get_flag(flag) == flag_state) {
		cpu.regs[PC].full = addr;
		cpu.cycle += 4;
	} else {
		cpu.regs[PC].full += 3;
		cpu.cycle += 3;
	}
}

void jp_aHL() {
	cpu.regs[PC].full = cpu.regs[HL].full;

	cpu.cycle += 1;
}

// Naming might be weird but that's how it's named in rgbds, so I went
// with that for consistency (it's easier to search in the docs this way).
// It's n16 because it jumps to address n16, but it uses an 8-bit offset instead.
void jr_n16(const int8_t offset) {
	cpu.regs[PC].full += 2;
	cpu.regs[PC].full += offset;
	cpu.cycle += 3;
}

// Naming might be weird but that's how it's named in rgbds, so I went
// with that for consistency (it's easier to search in the docs this way).
// It's n16 because it jumps to address n16, but it uses an 8-bit offset instead.
void jr_cc_n16(const int flag, const bool flag_state, const int8_t offset) {
	cpu.regs[PC].full += 2;
	if (get_flag(flag) == flag_state) {
		cpu.regs[PC].full += offset;
		cpu.cycle += 3;
	} else {
		cpu.cycle += 2;
	}
}

void rst(const uint8_t vec) {
	cpu.regs[SP].full -= 2;
	write16(cpu.regs[SP].full, cpu.regs[PC].full + 1);

	cpu.regs[PC].full = vec;
	cpu.cycle += 4;
}

void ret() {
	cpu.regs[PC].low = read8(cpu.regs[SP].full);
	++cpu.regs[SP].full;
	cpu.regs[PC].high = read8(cpu.regs[SP].full);
	++cpu.regs[SP].full;

	cpu.cycle += 4;
}

void ret_cc(const int flag, const bool flag_state) {
	if (get_flag(flag) == flag_state) {
		cpu.regs[PC].low = read8(cpu.regs[SP].full);
		++cpu.regs[SP].full;
		cpu.regs[PC].high = read8(cpu.regs[SP].full);
		++cpu.regs[SP].full;
		cpu.cycle += 5;
	} else {
		cpu.regs[PC].full += 1;
		cpu.cycle += 2;
	}
}

void reti() {
	cpu.ime = true;

	cpu.regs[PC].low = read8(cpu.regs[SP].full);
	++cpu.regs[SP].full;
	cpu.regs[PC].high = read8(cpu.regs[SP].full);
	++cpu.regs[SP].full;

	cpu.cycle += 4;
}

// ================ CARRY FLAG INSTRUCTIONS ================

void ccf() {
	set_flag(N, false);
	set_flag(H, false);
	get_flag(C) ? set_flag(C, false) : set_flag(C, true);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void scf() {
	set_flag(N, false);
	set_flag(H, false);
	set_flag(C, true);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

// ================ STACK INSTRUCTIONS ================

void pop_r16(uint16_t *src) {
	*src = read16(cpu.regs[SP].full);
	cpu.regs[SP].full += 2;

	cpu.regs[PC].full += 1;
	cpu.cycle += 3;
}

void pop_AF() {
	cpu.regs[AF].full = read16(cpu.regs[SP].full);
	cpu.regs[AF].low &= 0xF0;
	cpu.regs[SP].full += 2;

	cpu.regs[PC].full += 1;
	cpu.cycle += 3;
}

void push_r16(const uint16_t src) {
	cpu.regs[SP].full -= 2;
	write16(cpu.regs[SP].full, src);

	cpu.regs[PC].full += 1;
	cpu.cycle += 4;
}

void push_AF() {
	cpu.regs[SP].full -= 2;
	write16(cpu.regs[SP].full, cpu.regs[AF].full & 0xFFF0);

	cpu.regs[PC].full += 1;
	cpu.cycle += 4;
}

void ld_addr16_SP(const uint16_t addr) {
	write16(addr, cpu.regs[SP].full);

	cpu.regs[PC].full += 3;
	cpu.cycle += 5;
}

void ld_HL_SPe8(const int8_t val) {
	set_flag(Z, false);
	set_flag(N, false);
	(cpu.regs[SP].full & 0x000F) + (val & 0x0F) > 0x000F ? set_flag(H, true) : set_flag(H, false);
	(cpu.regs[SP].full & 0x00FF) + (val & 0xFF) > 0x00FF ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[HL].full = cpu.regs[SP].full + val;

	cpu.regs[PC].full += 2;
	cpu.cycle += 3;
}

void ld_SP_HL() {
	cpu.regs[SP].full = cpu.regs[HL].full;

	cpu.regs[PC].full += 1;
	cpu.cycle += 2;
}

void add_SP_n8(const int8_t val) {
	set_flag(Z, false);
	set_flag(N, false);
	(cpu.regs[SP].full & 0x000F) + (val & 0x0F) > 0x000F ? set_flag(H, true) : set_flag(H, false);
	(cpu.regs[SP].full & 0x00FF) + (val & 0xFF) > 0x00FF ? set_flag(C, true) : set_flag(C, false);

	cpu.regs[SP].full += val;

	cpu.regs[PC].full += 2;
	cpu.cycle += 4;
}

// ================ INTERRUPTS ================

void di() {
	cpu.ime = false;

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void ei() {
	cpu.ime_enable_counter = 1;

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void halt() {
	fprintf(stderr, "halt() is a stub - TODO implementation\n");
	usleep(1000); // 1ms

	cpu.regs[PC].full += 1;
}

// ================ MISC ================

void daa() {
	uint8_t adj = 0;
	bool sub = get_flag(N), hcarry = get_flag(H), carry = get_flag(C);

	if (sub) {
		if (hcarry) adj += 0x06;
		if (carry) adj += 0x60;
		adj > cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);
		cpu.regs[AF].high -= adj;
	} else {
		if (hcarry || (cpu.regs[AF].high & 0x0F) > 0x09) adj += 0x06;
		if (carry || cpu.regs[AF].high > 0x99) adj += 0x60;
		cpu.regs[AF].high + adj < cpu.regs[AF].high ? set_flag(C, true) : set_flag(C, false);
		cpu.regs[AF].high += adj;
	}

	set_flag(Z, !cpu.regs[AF].high);
	set_flag(H, false);

	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void nop() {
	cpu.regs[PC].full += 1;
	cpu.cycle += 1;
}

void stop_n8(const uint8_t val) {
	fprintf(stderr, "stop() is a stub - TODO implementation\n");
	// TODO: Remember about the divider register (0xFF04)
	usleep(1000000); // 1s

	cpu.regs[PC].full += 2;
}
