#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <stdbool.h>
#include <stdint.h>

enum RegisterNames {
  AF,
  BC,
  DE,
  HL,
  SP,
  PC,
  REGISTER_COUNT
};

// BC = Full 16 bit register
// B = High 8 bits
// C = Low 8 bits
union Register {
  uint16_t full;
  struct {
    uint8_t low;
    uint8_t high;
  };
};

typedef struct {
  uint8_t entry_point[4];
  uint8_t nitendo_logo[48];
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

extern union Register regs[REGISTER_COUNT];
extern bool display[160][144];
extern uint8_t ram[8192];
extern uint8_t vram[8192];
extern int cycle;

void set_zero_flag(bool n);
void set_subtraction_flag(bool n);
void set_half_carry_flag(bool n);
void set_carry_flag(bool n);

// https://rgbds.gbdev.io/docs/v0.9.2/gbz80.7
// naming is instruction_destination_source
// r8 - 8 bit register, r16 - 16 bit
// HL, A, C, SP - hard set registers
// a16 == [r16], addr16 == [n16]
// aHL = [HL], aC = [C]
// ==================================================
// TODO:
// some of these are probably redundant, since they will only
// be called in one case (loads with no dest or src operands)
// might be fine to keep them though, so that every instruction
// is executed the same way - through a function for it

// LOADS
void ld_r8_r8(uint8_t *dest, uint8_t *src);
void ld_r16_n16(uint16_t *dest, uint16_t src);
void ld_aHL_r8(uint8_t *src);
void ld_aHL_n8(uint8_t src);
void ld_r8_aHL(uint8_t *dest);
void ld_a16_A(uint16_t *dest);
void ld_addr16_A(uint16_t dest);
void ldh_addr16_A(uint16_t dest);
void ldh_aC_A();
void ld_A_a16(uint16_t *src);
void ld_A_addr16(uint16_t src);
void ldh_A_addr16(uint16_t src);
void ldh_A_aC();
void ld_aHLi_A();
void ld_aHLd_A();
void ld_A_aHLi();
void ld_A_aHLd();
void ld_SP_n16(uint16_t src);
void ld_addr16_SP(uint16_t dest);
void ld_aHL_SPe8();
void ld_SP_aHL();

// INTERRUPTS
void int_di();
void int_ei();
void int_halt();

#endif
