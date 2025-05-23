#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>

union Register regs[REGISTER_COUNT] = {0};

bool display[160][144] = {0};

uint8_t ram[8192] = {0};
uint8_t vram[8192] = {0};

void set_zero_flag(bool f) {
  if (f)
    regs[AF].low |= 0x80;
  else
    regs[AF].low &= ~0x80;
}

void set_subtraction_flag(bool f) {
  if (f)
    regs[AF].low |= 0x40;
  else
    regs[AF].low &= ~0x40;
}

void set_half_carry_flag(bool f) {
  if (f)
    regs[AF].low |= 0x20;
  else
    regs[AF].low &= ~0x20;
}

void set_carry_flag(bool f) {

  if (f)
    regs[AF].low |= 0x10;
  else
    regs[AF].low &= ~0x10;
}
