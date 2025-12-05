#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "constants.h"

// ================ HARDWARE ================

enum RegisterNames {
	AF,
	BC,
	DE,
	HL,
	SP, // always accessed as 16-bits
	PC, // always accessed as 16-bits
	REGISTER_COUNT
};

enum Flags {
	Z, // zero flag
	N, // subtract flag
	H, // half carry flag
	C, // carry flag
	FLAG_COUNT
};

// BC = Full 16 bit register
// B = High 8 bits
// C = Low 8 bits
typedef union {
	uint16_t full;
	struct {
		uint8_t low;
		uint8_t high;
	};
} Register;

typedef struct {
	uint8_t ram[8192]; // TODO: size change
	uint8_t vram[VRAM_SIZE];
	uint8_t io_registers[128];
	uint8_t oam[160];
	uint8_t hram[127];
	bool display[160][144];
} Memory_stuff;

typedef struct {
	Register regs[REGISTER_COUNT];
	uint64_t cycle;
	bool prefix;
	bool ime;
	int ime_enable_counter;
} CPU;

typedef struct {
	int div_cycle_counter;
	int tima_cycle_counter;
	bool tac_enable;
	// TAC's clock select - increment TIMA every:
	// (00) 256 cycles,
	// (01) 4 cycles,
	// (10) 16 cycles,
	// (11) 64 cycles
	int tac_increment_cycles;
} Timer_controls;

// ================ ROM STUFF ================

typedef struct {
	uint8_t entry_point[4];
	uint8_t nintendo_logo[48];
	char title[16]; // already includes manufacturer code
	uint8_t cgb_flag;
	uint16_t new_license_code;
	uint8_t sgb_flag;
	uint8_t cartridge_type;
	uint8_t rom_size;
	uint8_t ram_size;
	uint8_t destination_code;
	uint8_t old_licensee_code;
	uint8_t mask_rom_version_number;
	uint8_t header_checksum;
	uint16_t global_checksum;
} CartridgeHeader;

typedef struct {
	uint8_t *game_rom;
	size_t game_size;
	CartridgeHeader cartridge_header;
	bool boot_rom_enabled;
	const uint8_t boot_rom[256];
	uint8_t current_rom_bank;
	int max_rom_banks;
	uint8_t current_ram_bank;
	// multiplied by 4 because of possible RAM banking, if no
	// RAM banking then we simply can't access anything past 8192
	uint8_t external_ram[EXTERN_RAM_SIZE * 4];
} ROM;

typedef struct {
	bool ram_enable;
	uint8_t first_rom_bank_reg;
	uint8_t second_rom_bank_reg;
	uint8_t ram_bank_number;
	bool banking_mode_is_advanced; // 0 = simple, 1 = advanced
} MBC1_State;

void rom_write(const uint16_t addr, const uint8_t val);
void mbc1_write(const uint16_t addr, const uint8_t val);

uint8_t rom_read(const uint16_t addr);
uint8_t mbc1_read(const uint16_t addr);

// ================ HELPER FUNCTIONS ================

bool get_flag(const int flag);
void set_flag(const int flag, const bool value);

void write16(const uint16_t addr, const uint16_t val);
void write8(const uint16_t addr, const uint8_t val);
uint16_t read16(const uint16_t addr);
uint8_t read8(const uint16_t addr);

// ================ OPCODES ================

// LOADS
void ld_r8_r8(uint8_t *dest, const uint8_t src);
void ld_r8_n8(uint8_t *dest, const uint8_t val);
void ld_r16_n16(uint16_t *dest, const uint16_t val);
void ld_aHL_r8(const uint8_t src);
void ld_aHL_n8(const uint8_t val);
void ld_r8_aHL(uint8_t *dest);
void ld_a16_A(const uint16_t addr);
void ld_addr16_A(const uint16_t addr);
void ldh_addr16_A(const uint16_t addr);
void ldh_aC_A();
void ld_A_a16(const uint16_t addr);
void ld_A_addr16(const uint16_t addr);
void ldh_A_addr16(const uint16_t addr);
void ldh_A_aC();
void ld_aHLi_A();
void ld_aHLd_A();
void ld_A_aHLi();
void ld_A_aHLd();

// 8-BIT ARITHMETIC
void add_A_r8(const uint8_t src);
void add_A_aHL();
void add_A_n8(const uint8_t val);
void adc_A_r8(const uint8_t src);
void adc_A_aHL();
void adc_A_n8(const uint8_t val);
void sub_A_r8(const uint8_t src);
void sub_A_aHL();
void sub_A_n8(const uint8_t val);
void sbc_A_r8(const uint8_t src);
void sbc_A_aHL();
void sbc_A_n8(const uint8_t val);
void dec_r8(uint8_t *dest);
void dec_aHL();
void inc_r8(uint8_t *dest);
void inc_aHL();
void cp_A_r8(const uint8_t src);
void cp_A_aHL();
void cp_A_n8(const uint8_t val);

// 16-BIT ARITHMETIC
void add_HL_r16(const uint16_t src);
void dec_r16(uint16_t *dest);
void inc_r16(uint16_t *dest);

// BITWISE LOGIC
void and_A_r8(const uint8_t src);
void and_A_aHL();
void and_A_n8(const uint8_t val);
void or_A_r8(const uint8_t src);
void or_A_aHL();
void or_A_n8(const uint8_t val);
void cpl();
void xor_A_r8(const uint8_t src);
void xor_A_aHL();
void xor_A_n8(const uint8_t val);

// BIT FLAGS
void bit_u3_r8(const int bit_num, const uint8_t src);
void bit_u3_aHL(const int bit_num);
void res_u3_r8(const int bit_num, uint8_t *src);
void res_u3_aHL(const int bit_num);
void set_u3_r8(const int bit_num, uint8_t *src);
void set_u3_aHL(const int bit_num);

// BIT SHIFTS
void rla();
void rlca();
void rra();
void rrca();
void rlc_r8(uint8_t *src);
void rlc_aHL();
void rrc_r8(uint8_t *src);
void rrc_aHL();
void rl_r8(uint8_t *src);
void rl_aHL();
void rr_r8(uint8_t *src);
void rr_aHL();
void sla_r8(uint8_t *src);
void sla_aHL();
void sra_r8(uint8_t *src);
void sra_aHL();
void swap_r8(uint8_t *src);
void swap_aHL();
void srl_r8(uint8_t *src);
void srl_aHL();

// JUMPS
void call_n16(const uint16_t addr);
void call_cc_n16(const int flag, const bool flag_state, const uint16_t addr);
void jp_n16(const uint16_t addr);
void jp_cc_n16(const int flag, const bool flag_state, const uint16_t addr);
void jp_aHL();
void jr_n16(const int8_t offset);
void jr_cc_n16(const int flag, const bool flag_state, const int8_t offset);
void rst(const uint8_t vec);
void ret();
void ret_cc(const int flag, const bool flag_state);
void reti();

// CARRY FLAG INSTRUCTIONS
void ccf();
void scf();

// STACK INSTRUCTIONS
void pop_r16(uint16_t *src);
void pop_AF();
void push_r16(const uint16_t src);
void push_AF();
void ld_addr16_SP(const uint16_t addr);
void ld_HL_SPe8(const int8_t val);
void ld_SP_HL();
void add_SP_n8(const int8_t val);

// INTERRUPTS
void di();
void ei();
void halt();

// MISC
void daa();
void nop();
void stop_n8(const uint8_t val);

#endif
