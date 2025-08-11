#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// ================ HARDWARE ================

Register regs[REGISTER_COUNT] = {0};
bool display[160][144] = {0};
uint8_t ram[8192] = {0};
uint8_t vram[8192] = {0};
uint8_t io_registers[128] = {0};
uint8_t oam[160] = {0};
CartridgeHeader cartridge_header = {0};
int cycle = 0;
bool prefix = false;

// ================ ROM STUFF ================

// bootix bootrom: https://github.com/Hacktix/Bootix
uint8_t boot_rom[256] = {
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
    0x3e, 0x01, 0xe0, 0x50};

int current_rom_bank = 0;

// TODO
void switch_rom_bank();

// ================ HELPER FUNCTIONS ================

bool get_flag(int flag) {
  switch (flag) {
  case Z:
    return (regs[AF].low & 0x80);
  case N:
    return (regs[AF].low & 0x40);
  case H:
    return (regs[AF].low & 0x20);
  case C:
    return (regs[AF].low & 0x10);
  default:
    fprintf(stderr, "check_flag() got an incorrect flag int (flags are 0-3 in the Flags enum)\n");
    return false;
  }
}

void set_flag(int flag, bool value) {
  switch (flag) {
  case Z:
    if (value)
      regs[AF].low |= 0x80;
    else
      regs[AF].low &= ~0x80;
    return;
  case N:
    if (value)
      regs[AF].low |= 0x40;
    else
      regs[AF].low &= ~0x40;
    return;
  case H:
    if (value)
      regs[AF].low |= 0x20;
    else
      regs[AF].low &= ~0x20;
    return;
  case C:
    if (value)
      regs[AF].low |= 0x10;
    else
      regs[AF].low &= ~0x10;
    return;
  default:
    fprintf(stderr, "set_flag() got an incorrect flag int (flags are 0-3 in the Flags enum)\n");
    return;
  }
}

void write16(uint16_t addr, uint16_t val) {
  write8(addr, val);
  write8(addr + 1, val >> 8);
}

uint16_t read16(uint16_t addr) {
  return read8(addr) | (read8(addr + 1) << 8);
}

void write8(uint16_t addr, uint8_t val) {
  if (addr < ROM_BANK_N_ADDR)
    ;
  else if (addr < VRAM_ADDR)
    ;
  else if (addr < EXTERN_RAM_ADDR)
    vram[addr - VRAM_ADDR] = val;
  else if (addr < WRAM_0_ADDR)
    ;
  else if (addr < WRAM_N_ADDR)
    ram[addr - WRAM_0_ADDR] = val;
  else if (addr < ECHO_RAM_ADDR)
    ram[addr - WRAM_0_ADDR] = val;
  else if (addr < OAM_ADDR)
    fprintf(stderr, "write8: use of echo ram is prohibited\n");
  else if (addr < INVAL_MEM_ADDR)
    oam[addr - OAM_ADDR] = val;
  else if (addr < IO_REGS_ADDR)
    fprintf(stderr, "write8: use of 0xFEA0-0xFEFF is prohibited\n");
  else if (addr < HRAM_ADDR)
    ;
  else if (addr < INT_ENABLE_ADDR)
    ;
  else
    ;
}

uint8_t read8(uint16_t addr) {
  if (addr < ROM_BANK_N_ADDR)
    ;
  else if (addr < VRAM_ADDR)
    ;
  else if (addr < EXTERN_RAM_ADDR)
    return vram[addr - VRAM_ADDR];
  else if (addr < WRAM_0_ADDR)
    ;
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
  } else if (addr < HRAM_ADDR)
    return io_registers[addr - IO_REGS_ADDR];
  else if (addr < INT_ENABLE_ADDR)
    ;
  else
    ;

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

void ld_r8_r8(uint8_t *dest, uint8_t *src) {
  *dest = *src;

  regs[PC].full += 1;
  cycle += 1;
}

void ld_r8_n8(uint8_t *dest, uint8_t val) {
  *dest = val;

  regs[PC].full += 2;
  cycle += 2;
}

void ld_r16_n16(uint16_t *dest, uint16_t val) {
  *dest = val;

  regs[PC].full += 3;
  cycle += 3;
}

void ld_aHL_r8(uint8_t *src) {
  write8(regs[HL].full, *src);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_aHL_n8(uint8_t src);

void ld_r8_aHL(uint8_t *dest) {
  *dest = read8(regs[HL].full);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_a16_A(uint16_t addr) {
  // TODO: need this one for the serial write - I need to copy it
  // if (addr < 0x4000)
  //   ;
  // else if (addr < 0x8000)
  //   ;
  // else if (addr < 0xA000)
  //   ;
  // else if (addr < 0xC000)
  //   ;
  // else if (addr < 0xD000)
  //   ;
  // else if (addr < 0xE000)
  //   ;
  // else if (addr < 0xFE00)
  //   ;
  // else if (addr < 0xFEA0)
  //   ;
  // else if (addr < 0xFF00)
  //   ;
  // else if (addr < 0xFF80) {
  //   io_registers[addr - IO_REGS] = regs[AF].high;
  //   if (addr == SERIAL_TRANSFER)
  //     printf("%c", regs[AF].high);
  // } else if (addr < 0xFFFF)
  //   ;
  // else
  //   ;

  write8(addr, regs[AF].high);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_addr16_A(uint16_t addr);
void ldh_addr16_A(uint16_t addr);

void ldh_aC_A() {
}

void ld_A_a16(uint16_t addr);
void ld_A_addr16(uint16_t addr);
void ldh_A_addr16(uint16_t addr);
void ldh_A_aC();
void ld_aHLi_A();

void ld_aHLd_A() {
  write8(regs[HL].full, regs[AF].high);
  --regs[HL].full;

  regs[PC].full += 1;
  cycle += 2;
}

void ld_A_aHLi();
void ld_A_aHLd();

void ld_SP_n16(uint16_t val) {
  regs[SP].full = val;

  regs[PC].full += 3;
  cycle += 3;
}

void ld_addr16_SP(uint16_t addr) {
  write16(addr, regs[SP].full);

  regs[PC].full += 3;
  cycle += 5;
}

void ld_aHL_SPe8();
void ld_SP_aHL();

// ================ BITWISE ================

void xor_A_r8(uint8_t *src) {
  regs[AF].high ^= *src;

  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, false);
  set_flag(C, false);

  regs[PC].full += 1;
  cycle += 1;
}

// ================ BIT FLAGS ================

void bit_u3_r8(int bit_num, uint8_t *src) {
  *src & (1 << bit_num) ? set_flag(Z, false) : set_flag(Z, true);

  set_flag(N, false);
  set_flag(H, true);

  regs[PC].full += 1;
  cycle += 2;
}

void bit_u3_aHL(int bit_num) {

  read8(regs[HL].full) & (1 << bit_num) ? set_flag(Z, false) : set_flag(Z, true);

  set_flag(N, false);
  set_flag(H, true);

  regs[PC].full += 1;
  cycle += 2;
}

// ================ JUMPS ================

// TODO
void jr_n16(uint16_t dest) {
  cycle += 3;
}

// naming might be weird but that's how it's named in rgbds, so I went
// with that for consistency (it's easier to search in the docs this way)
// it's n16 because it jumps to address n16, but it uses an 8-bit offset instead
void jr_cc_n16(int flag, bool flag_state, int8_t offset) {
  regs[PC].full += 2;
  if (get_flag(flag) == flag_state) {
    regs[PC].full += offset;
    cycle += 3;
  } else {
    cycle += 2;
  }
}

// ================ INTERRUPTS ================

void int_di();
void int_ei();
// TODO: implement this one, since it's already used in main.c
void int_halt();

// ================ MISC ================

void nop() {
  regs[PC].full += 1;
  cycle += 1;
}
