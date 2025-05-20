#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>

union Reg gp_regs[4] = {0};      // general purpose registers
uint16_t af = 0, sp = 0, pc = 0; // accumulator & flags, stack pointer, program counter

bool display[160][144] = {0};

uint8_t ram[8192] = {0};
uint8_t vram[8192] = {0};
