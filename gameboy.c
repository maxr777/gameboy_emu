#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>

Register regs[REGISTER_COUNT] = {0};
CartridgeHeader cartridge_header = {0};
bool display[160][144] = {0};
uint8_t ram[8192] = {0};
uint8_t vram[8192] = {0};
int cycle = 0;

void set_zero_flag(bool f) {
  if (f) {
    regs[AF].low |= 0x80;
  } else {
    regs[AF].low &= ~0x80;
  }
}

void set_subtraction_flag(bool f) {
  if (f) {
    regs[AF].low |= 0x40;
  } else {
    regs[AF].low &= ~0x40;
  }
}

void set_half_carry_flag(bool f) {
  if (f) {
    regs[AF].low |= 0x20;
  } else {
    regs[AF].low &= ~0x20;
  }
}

void set_carry_flag(bool f) {
  if (f) {
    regs[AF].low |= 0x10;
  } else {
    regs[AF].low &= ~0x10;
  }
}

// ================ LOADS ================

void ld_r8_r8(uint8_t *dest, uint8_t *src) {
  *dest = *src;
  ++cycle;
}

void ld_r16_n16(uint16_t *dest, uint16_t src) {
  *dest = src;
  cycle += 3;
}

void ld_aHL_r8(uint8_t *src) {
  ram[regs[HL].full] = *src;
  cycle += 2;
}

void ld_aHL_n8(uint8_t src);

void ld_r8_aHL(uint8_t *dest) {
  *dest = ram[regs[HL].full];
  cycle += 2;
}

void ld_a16_A(uint16_t *dest) {
  ram[*dest] = regs[AF].high;
  cycle += 2;
}

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

void ld_addr16_SP(uint16_t dest) {
  ram[dest] = regs[SP].full;
  cycle += 4;
}

void ld_aHL_SPe8();
void ld_SP_aHL();

// ================ INTERRUPTS ================

void int_di();
void int_ei();
// TODO: implement this one, since it's already used in main.c
void int_halt();
