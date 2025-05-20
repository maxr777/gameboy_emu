#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <stdint.h>

union Reg {
  uint16_t full;
  struct {
    uint8_t low;
    uint8_t high;
  };
};

extern union Reg gp_regs[4]; // general purpose registers
extern uint16_t af, sp, pc;  // accumulator & flags, stack pointer, program counter

extern bool display[160][144];

extern uint8_t ram[8192];
extern uint8_t vram[8192];

#endif
