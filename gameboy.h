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

extern union Register regs[REGISTER_COUNT];
extern bool display[160][144];
extern uint8_t ram[8192];
extern uint8_t vram[8192];

void set_zero_flag(int n);
void set_subtraction_flag(int n);
void set_half_carry_flag(int n);
void set_carry_flag(int n);

#endif
