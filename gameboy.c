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

uint8_t *game_rom; // initialized in main.c
size_t game_size;  // initialized in main.c

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

void rom_write(const uint16_t addr, const uint8_t val) {
  switch (cartridge_header.cartridge_type) {
  case 0x00:
    fprintf(stderr, "Writing to ROM with MCB0 is prohibited\n");
    break;
  case 0x01:
  case 0x02:
  case 0x03:
    mcb1_write(addr, val);
    break;
  default:
    fprintf(stderr, "rom_write(): Unimplemented MCB\n");
    break;
  }
}

void mcb1_write(const uint16_t addr, const uint8_t val) {
  fprintf(stderr, "mcb1_write(): Not yet implemented\n");
}

uint8_t rom_read(const uint16_t addr) {
  switch (cartridge_header.cartridge_type) {
  case 0x00:
    return game_rom[addr];
  case 0x01:
  case 0x02:
  case 0x03:
    return mcb1_read(addr);
  default:
    fprintf(stderr, "rom_read(): Unimplemented MCB\n");
    return 0;
  }
}

uint8_t mcb1_read(const uint16_t addr) {
  fprintf(stderr, "mcb1_read(): Not yet implemented\n");
  return 0;
}

// ================ ROM STUFF ================

bool ime = false;
// Since EI (ime = true instruction) works after a 1 instruction long delay, there is a need for a counter.
// If 1, then decrement.
// If 0, then decrement and set ime to true.
// If -1, then do nothing.
// The default state is -1. When EI gets called, it gets set to 1 and decrements from there.
int ime_enable_counter = -1;

// ================ HELPER FUNCTIONS ================

bool get_flag(const int flag) {
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

void set_flag(const int flag, const bool value) {
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
  else if (addr < HRAM_ADDR) {
    io_registers[addr - IO_REGS_ADDR] = val;
    if (addr == SERIAL_TRANSFER)
      printf("%c", val);
  } else if (addr < INT_ENABLE_ADDR)
    ;
  else
    ;
}

uint8_t read8(const uint16_t addr) {
  if (addr < ROM_BANK_N_ADDR)
    return rom_read(addr);
  else if (addr < VRAM_ADDR)
    return rom_read(addr);
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

void ld_r8_r8(uint8_t *dest, const uint8_t src) {
  *dest = src;

  regs[PC].full += 1;
  cycle += 1;
}

void ld_r8_n8(uint8_t *dest, const uint8_t val) {
  *dest = val;

  regs[PC].full += 2;
  cycle += 2;
}

void ld_r16_n16(uint16_t *dest, const uint16_t val) {
  *dest = val;

  regs[PC].full += 3;
  cycle += 3;
}

void ld_aHL_r8(const uint8_t src) {
  write8(regs[HL].full, src);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_aHL_n8(const uint8_t val) {
  write8(regs[HL].full, val);

  regs[PC].full += 2;
  cycle += 3;
}

void ld_r8_aHL(uint8_t *dest) {
  *dest = read8(regs[HL].full);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_a16_A(const uint16_t addr) {
  write8(addr, regs[AF].high);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_addr16_A(const uint16_t addr) {
  write8(addr, regs[AF].high);

  regs[PC].full += 3;
  cycle += 4;
}

void ldh_addr16_A(const uint16_t addr) {
  if (addr >= 0xFF00 && addr <= 0xFFFF)
    write8(addr, regs[AF].high);

  regs[PC].full += 2;
  cycle += 3;
}

void ldh_aC_A() {
  write8(0xFF00 + regs[BC].low, regs[AF].high);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_A_a16(uint16_t addr) {
  regs[AF].high = read8(addr);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_A_addr16(uint16_t addr) {
  regs[AF].high = read8(addr);

  regs[PC].full += 3;
  cycle += 4;
}

void ldh_A_addr16(const uint16_t addr) {
  if (addr >= 0xFF00 && addr <= 0xFFFF)
    regs[AF].high = read8(addr);

  regs[PC].full += 2;
  cycle += 3;
}

void ldh_A_aC() {
  regs[AF].high = read8(0xFF00 + regs[BC].low);

  regs[PC].full += 1;
  cycle += 2;
}

void ld_aHLi_A() {
  write8(regs[HL].full, regs[AF].high);
  ++regs[HL].full;

  regs[PC].full += 1;
  cycle += 2;
}

void ld_aHLd_A() {
  write8(regs[HL].full, regs[AF].high);
  --regs[HL].full;

  regs[PC].full += 1;
  cycle += 2;
}

void ld_A_aHLi() {
  regs[AF].high = read8(regs[HL].full);
  ++regs[HL].full;

  regs[PC].full += 1;
  cycle += 2;
}

void ld_A_aHLd() {
  regs[AF].high = read8(regs[HL].full);
  --regs[HL].full;

  regs[PC].full += 1;
  cycle += 2;
}

// ================ 8-BIT ARITHMETIC ================

void add_A_r8(const uint8_t src) {
  set_flag(N, false);

  (regs[AF].high & 0x0F) + (src & 0x0F) > 0x0F ? set_flag(H, true) : set_flag(H, false);

  (regs[AF].high + src) > regs[AF].high ? set_flag(C, true) : set_flag(C, false);

  regs[AF].high += src;
  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);

  regs[PC].full += 1;
  cycle += 1;
}

void adc_A_r8(const uint8_t src) {
  set_flag(N, false);
  bool c = get_flag(C);

  (regs[AF].high & 0x0F) + (src & 0x0F) + c > 0x0F ? set_flag(H, true) : set_flag(H, false);
  (uint16_t)regs[AF].high + src + c > 0x00FF ? set_flag(C, true) : set_flag(C, false);

  regs[AF].high += src + c;
  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);

  regs[PC].full += 1;
  cycle += 1;
}

void sub_A_r8(const uint8_t src) {
  set_flag(N, true);

  src > (regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
  src > regs[AF].high ? set_flag(C, true) : set_flag(C, false);

  regs[AF].high -= src;
  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);

  regs[PC].full += 1;
  cycle += 1;
}

void sbc_A_r8(const uint8_t src) {
  set_flag(N, true);
  bool c = get_flag(C);

  src + c > (regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
  src + c > regs[AF].high ? set_flag(C, true) : set_flag(C, false);

  regs[AF].high -= (src + c);
  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);

  regs[PC].full += 1;
  cycle += 1;
}

void dec_r8(uint8_t *dest) {
  // lower 4 bits only borrow if they're 0000
  ((*dest)-- & 0x0F) == 0x00 ? set_flag(H, true) : set_flag(H, false);

  *dest == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, true);

  regs[PC].full += 1;
  cycle += 1;
}

void dec_aHL() {
  uint8_t result = read8(regs[HL].full);

  (result-- & 0x0F) == 0x00 ? set_flag(H, true) : set_flag(H, false);
  result == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, true);

  write8(regs[HL].full, result);

  regs[PC].full += 1;
  cycle += 3;
}

void inc_r8(uint8_t *dest) {
  ((*dest)++ & 0x0F) == 0x0F ? set_flag(H, true) : set_flag(H, false);

  *dest == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, false);

  regs[PC].full += 1;
  cycle += 1;
}

void inc_aHL() {
  uint8_t result = read8(regs[HL].full);

  (result++ & 0x0F) == 0x0F ? set_flag(H, true) : set_flag(H, false);
  result == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, false);

  write8(regs[HL].full, result);

  regs[PC].full += 1;
  cycle += 3;
}

void cp_A_r8(const uint8_t src) {
  uint8_t result = regs[AF].high - src;

  result == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, true);
  src > (regs[AF].high & 0x0F) ? set_flag(H, true) : set_flag(H, false);
  src > regs[AF].high ? set_flag(C, true) : set_flag(C, false);

  regs[PC].full += 1;
  cycle += 1;
}

// ================ 16-BIT ARITHMETIC ================

void add_HL_r16(const uint16_t src) {
  set_flag(N, false);

  (regs[HL].full + src) > regs[HL].full ? set_flag(C, true) : set_flag(C, false);
  (regs[HL].full & 0x0FFF) + (src & 0x0FFF) > 0x0FFF ? set_flag(H, true) : set_flag(H, false);

  regs[HL].full += src;

  regs[PC].full += 1;
  cycle += 2;
}

void dec_r16(uint16_t *dest) {
  --(*dest);

  regs[PC].full += 1;
  cycle += 2;
}

void inc_r16(uint16_t *dest) {
  ++(*dest);

  regs[PC].full += 1;
  cycle += 2;
}

// ================ BITWISE LOGIC ================

void and_A_r8(const uint8_t src) {
  regs[AF].high &= src;

  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, true);
  set_flag(C, false);

  regs[PC].full += 1;
  cycle += 1;
}

void or_A_r8(const uint8_t src) {
  regs[AF].high |= src;

  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, false);
  set_flag(C, false);

  regs[PC].full += 1;
  cycle += 1;
}

void cpl() {
  regs[AF].high = ~regs[AF].high;

  set_flag(N, true);
  set_flag(H, true);

  regs[PC].full += 1;
  cycle += 1;
}

void xor_A_r8(const uint8_t src) {
  regs[AF].high ^= src;

  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, false);
  set_flag(C, false);

  regs[PC].full += 1;
  cycle += 1;
}

// ================ BIT FLAGS ================

void bit_u3_r8(const int bit_num, const uint8_t src) {
  src & (1 << bit_num) ? set_flag(Z, false) : set_flag(Z, true);

  set_flag(N, false);
  set_flag(H, true);

  regs[PC].full += 1;
  cycle += 2;
}

void bit_u3_aHL(const int bit_num) {

  read8(regs[HL].full) & (1 << bit_num) ? set_flag(Z, false) : set_flag(Z, true);

  set_flag(N, false);
  set_flag(H, true);

  regs[PC].full += 1;
  cycle += 2;
}

// ================ BIT SHIFTS ================

void rla() {
  set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, false);

  bool old_carry = get_flag(C);
  bool new_carry = (regs[AF].high & 0x80) == 0x80;
  set_flag(C, new_carry);

  regs[AF].high <<= 1;

  if (old_carry)
    regs[AF].high |= 1;
  else
    regs[AF].high &= ~1;

  regs[PC].full += 1;
  cycle += 1;
}

void rlca() {
  set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, false);

  bool carry = (regs[AF].high & 0x80) == 0x80;
  set_flag(C, carry);

  regs[AF].high <<= 1;

  if (carry)
    regs[AF].high |= 1;
  else
    regs[AF].high &= ~1;

  regs[PC].full += 1;
  cycle += 1;
}

void rra() {
  set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, false);

  bool old_carry = get_flag(C);
  bool new_carry = (regs[AF].high & 0x01) == 0x01;
  set_flag(C, new_carry);

  regs[AF].high >>= 1;

  if (old_carry)
    regs[AF].high |= (1 << 7);
  else
    regs[AF].high &= ~(1 << 7);

  regs[PC].full += 1;
  cycle += 1;
}

void rrca() {
  set_flag(Z, false);
  set_flag(N, false);
  set_flag(H, false);

  bool carry = (regs[AF].high & 0x01) == 0x01;
  set_flag(C, carry);

  regs[AF].high >>= 1;
  if (carry)
    regs[AF].high |= (1 << 7);
  else
    regs[AF].high &= ~(1 << 7);

  regs[PC].full += 1;
  cycle += 1;
}

// ================ JUMPS ================

void call_n16(const uint16_t addr) {
  regs[SP].full -= 2;
  write16(regs[SP].full, regs[PC].full + 3);

  regs[PC].full = addr;

  cycle += 6;
}

void call_cc_n16(const int flag, const bool flag_state, const uint16_t addr) {
  if (get_flag(flag) == flag_state) {
    regs[SP].full -= 2;
    write16(regs[SP].full, regs[PC].full + 3);
    regs[PC].full = addr;
    cycle += 6;
  } else {
    regs[PC].full += 3;
    cycle += 3;
  }
}

void jp_n16(const uint16_t addr) {
  regs[PC].full = addr;

  cycle += 4;
}

void jp_cc_n16(const int flag, const bool flag_state, const uint16_t addr) {
  if (get_flag(flag) == flag_state) {
    regs[PC].full = addr;
    cycle += 4;
  } else {
    regs[PC].full += 3;
    cycle += 3;
  }
}

void jp_aHL() {
  regs[PC].full = regs[HL].full;

  cycle += 1;
}

// Naming might be weird but that's how it's named in rgbds, so I went
// with that for consistency (it's easier to search in the docs this way).
// It's n16 because it jumps to address n16, but it uses an 8-bit offset instead.
void jr_n16(const int8_t offset) {
  regs[PC].full += 2;
  regs[PC].full += offset;
  cycle += 3;
}

// Naming might be weird but that's how it's named in rgbds, so I went
// with that for consistency (it's easier to search in the docs this way).
// It's n16 because it jumps to address n16, but it uses an 8-bit offset instead.
void jr_cc_n16(const int flag, const bool flag_state, const int8_t offset) {
  regs[PC].full += 2;
  if (get_flag(flag) == flag_state) {
    regs[PC].full += offset;
    cycle += 3;
  } else {
    cycle += 2;
  }
}

void rst(const uint8_t vec) {
  regs[SP].full -= 2;
  write16(regs[SP].full, regs[PC].full + 1);

  regs[PC].full = vec;
  cycle += 4;
}

void ret() {
  regs[PC].low = read8(regs[SP].full);
  ++regs[SP].full;
  regs[PC].high = read8(regs[SP].full);
  ++regs[SP].full;

  cycle += 4;
}

void ret_cc(const int flag, const bool flag_state) {
  if (get_flag(flag) == flag_state) {
    regs[PC].low = read8(regs[SP].full);
    ++regs[SP].full;
    regs[PC].high = read8(regs[SP].full);
    ++regs[SP].full;
    cycle += 5;
  } else {
    regs[PC].full += 1;
    cycle += 2;
  }
}

void reti() {
  ime = true;

  regs[PC].low = read8(regs[SP].full);
  ++regs[SP].full;
  regs[PC].high = read8(regs[SP].full);
  ++regs[SP].full;

  cycle += 4;
}

// ================ CARRY FLAG INSTRUCTIONS ================

void ccf() {
  set_flag(N, false);
  set_flag(H, false);
  get_flag(C) ? set_flag(C, false) : set_flag(C, true);

  regs[PC].full += 1;
  cycle += 1;
}

void scf() {
  set_flag(N, false);
  set_flag(H, false);
  set_flag(C, true);

  regs[PC].full += 1;
  cycle += 1;
}

// ================ STACK INSTRUCTIONS ================

void pop_r16(uint16_t *src) {
  *src = read16(regs[SP].full);
  regs[SP].full += 2;

  regs[PC].full += 1;
  cycle += 3;
}

void push_r16(const uint16_t src) {
  regs[SP].full -= 2;
  write16(regs[SP].full, src);

  regs[PC].full += 1;
  cycle += 4;
}

void ld_addr16_SP(const uint16_t addr) {
  write16(addr, regs[SP].full);

  regs[PC].full += 3;
  cycle += 5;
}

void ld_HL_SPe8(const int8_t val) {
  set_flag(Z, false);
  set_flag(N, false);
  (regs[SP].full & 0x000F) + (val & 0x0F) > 0x000F ? set_flag(H, true) : set_flag(H, false);
  (regs[SP].full & 0x00FF) + (val & 0xFF) > 0x00FF ? set_flag(C, true) : set_flag(C, false);

  regs[HL].full = regs[SP].full + val;

  regs[PC].full += 2;
  cycle += 3;
}

void ld_SP_HL() {
  regs[SP].full = regs[HL].full;

  regs[PC].full += 1;
  cycle += 2;
}

// ================ INTERRUPTS ================

void di() {
  ime = false;

  regs[PC].full += 1;
  cycle += 1;
}

void ei() {
  ime_enable_counter = 1;

  regs[PC].full += 1;
  cycle += 1;
}

// TODO: implement this one, since it's already used in main.c
void halt();

// ================ MISC ================

void daa() {
  uint8_t adj = 0;
  bool sub = get_flag(N), hcarry = get_flag(H), carry = get_flag(C);

  if (sub) {
    if (hcarry) adj += 0x06;
    if (carry) adj += 0x60;
    adj > regs[AF].high ? set_flag(C, true) : set_flag(C, false);
    regs[AF].high -= adj;
  } else {
    if (hcarry || (regs[AF].high & 0x0F) > 0x09) adj += 0x06;
    if (carry || regs[AF].high > 0x99) adj += 0x60;
    regs[AF].high + adj < regs[AF].high ? set_flag(C, true) : set_flag(C, false);
    regs[AF].high += adj;
  }

  regs[AF].high == 0 ? set_flag(Z, true) : set_flag(Z, false);
  set_flag(H, false);

  regs[PC].full += 1;
  cycle += 1;
}

void nop() {
  regs[PC].full += 1;
  cycle += 1;
}
