#include "gameboy.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define DISP_MULTP 4

// TODO: add debug mode argument
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "No game loaded");
    return 1;
  }

  bool debug = false;

  FILE *game_rom = fopen(argv[1], "rb");
  if (!game_rom) {
    perror("fopen game fail:");
    return 1;
  }
  fseek(game_rom, 256, SEEK_SET);

  FILE *boot_rom = fopen("bootix_dmg.bin", "rb");
  if (!boot_rom) {
    perror("fopen bootrom fail:");
    return 1;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Gameboy", 160 * DISP_MULTP, 144 * DISP_MULTP, 0);
  if (!window) {
    fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Event event;
  bool running = true;
  uint8_t byte = 0;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
    }

    if (regs[PC].full < 0x0100)
      fread(&byte, sizeof(byte), 1, boot_rom);
    else if (regs[PC].full == 0x0100)
      // TODO: this probably doesn't work because of padding
      fread(&cartridge_header, sizeof(cartridge_header), 1, game_rom);
    else
      fread(&byte, sizeof(byte), 1, game_rom);

    switch (byte) {
    case 0x00: // NOP
      if (debug) printf("0x%02X\t%s\n", byte, "NOP");
      ++cycle;
      break;
    case 0x01: // LD BC, n16
    {
      uint16_t n16;
      fread(&n16, 2, 1, game_rom);
      if (debug) printf("0x%02X %04X\t%s\n", byte, n16, "LD BC, n16");
      ld_r16_n16(&regs[BC].full, n16);
    } break;
    case 0x02: // LD [BC], A
      ld_a16_A(&regs[BC].full);
      break;
    case 0x03: // INC BC
      ++regs[BC].full;
      cycle += 2;
      break;
    case 0x04: // INC B
      ++regs[BC].high;
      ++cycle;
      break;
    case 0x05: // DEC B
    {
      uint8_t old = regs[BC].high;
      uint8_t result = --regs[BC].high;
      set_half_carry_flag((old & 0x0f) < (result & 0x0f));
      set_zero_flag(result == 0);
      set_subtraction_flag(true);
      ++cycle;
    } break;
    case 0x06: // LD B, n8
      fread(&regs[BC].high, 1, 1, game_rom);
      cycle += 2;
      break;
    case 0x07: // RLCA
    {
      bool result = regs[AF].high & 0x80;
      set_carry_flag(result);
      regs[AF].high <<= 1;
      regs[AF].high = regs[AF].high | result;
      set_zero_flag(false);
      set_subtraction_flag(false);
      set_half_carry_flag(false);
      ++cycle;
    } break;
    case 0x08: // LD [n16], SP
    {
      uint16_t address;
      fread(&address, 2, 1, game_rom);
      ld_addr16_SP(address);
    } break;
    case 0x09:
      break;
    case 0x40: // LD B, B
      ld_r8_r8(&regs[BC].high, &regs[BC].high);
      break;
    case 0x41: // LD B, C
      ld_r8_r8(&regs[BC].high, &regs[BC].low);
      break;
    case 0x42: // LD B, D
      ld_r8_r8(&regs[BC].high, &regs[DE].high);
      break;
    case 0x43: // LD B, E
      ld_r8_r8(&regs[BC].high, &regs[DE].low);
      break;
    case 0x44: // LD B, H
      ld_r8_r8(&regs[BC].high, &regs[HL].high);
      break;
    case 0x45: // LD B, L
      ld_r8_r8(&regs[BC].high, &regs[HL].low);
      break;
    case 0x46: // LD B, [HL]
      ld_r8_aHL(&regs[BC].high);
      break;
    case 0x47: // LD B, A
      ld_r8_r8(&regs[BC].high, &regs[AF].high);
      break;
    case 0x48: // LD C, B
      ld_r8_r8(&regs[BC].low, &regs[BC].high);
      break;
    case 0x49: // LD C, C
      ld_r8_r8(&regs[BC].low, &regs[BC].low);
      break;
    case 0x4A: // LD C, D
      ld_r8_r8(&regs[BC].low, &regs[DE].high);
      break;
    case 0x4B: // LD C, E
      ld_r8_r8(&regs[BC].low, &regs[DE].low);
      break;
    case 0x4C: // LD C, H
      ld_r8_r8(&regs[BC].low, &regs[HL].high);
      break;
    case 0x4D: // LD C, L
      ld_r8_r8(&regs[BC].low, &regs[HL].low);
      break;
    case 0x4E: // LD C, [HL]
      ld_r8_aHL(&regs[BC].low);
      break;
    case 0x4F: // LD C, A
      ld_r8_r8(&regs[BC].low, &regs[AF].high);
      break;
    case 0x50: // LD D, B
      ld_r8_r8(&regs[DE].high, &regs[BC].high);
      break;
    case 0x51: // LD D, C
      ld_r8_r8(&regs[DE].high, &regs[BC].low);
      break;
    case 0x52: // LD D, D
      ld_r8_r8(&regs[DE].high, &regs[DE].high);
      break;
    case 0x53: // LD D, E
      ld_r8_r8(&regs[DE].high, &regs[DE].low);
      break;
    case 0x54: // LD D, H
      ld_r8_r8(&regs[DE].high, &regs[HL].high);
      break;
    case 0x55: // LD D, L
      ld_r8_r8(&regs[DE].high, &regs[HL].low);
      break;
    case 0x56: // LD D, [HL]
      ld_r8_aHL(&regs[DE].high);
      break;
    case 0x57: // LD D, A
      ld_r8_r8(&regs[DE].high, &regs[AF].high);
      break;
    case 0x58: // LD E, B
      ld_r8_r8(&regs[DE].low, &regs[BC].high);
      break;
    case 0x59: // LD E, C
      ld_r8_r8(&regs[DE].low, &regs[BC].low);
      break;
    case 0x5A: // LD E, D
      ld_r8_r8(&regs[DE].low, &regs[DE].high);
      break;
    case 0x5B: // LD E, E
      ld_r8_r8(&regs[DE].low, &regs[DE].low);
      break;
    case 0x5C: // LD E, H
      ld_r8_r8(&regs[DE].low, &regs[HL].high);
      break;
    case 0x5D: // LD E, L
      ld_r8_r8(&regs[DE].low, &regs[HL].low);
      break;
    case 0x5E: // LD E, [HL]
      ld_r8_aHL(&regs[DE].low);
      break;
    case 0x5F: // LD E, A
      ld_r8_r8(&regs[DE].low, &regs[AF].high);
      break;
    case 0x60: // LD H, B
      ld_r8_r8(&regs[HL].high, &regs[BC].high);
      break;
    case 0x61: // LD H, C
      ld_r8_r8(&regs[HL].high, &regs[BC].low);
      break;
    case 0x62: // LD H, D
      ld_r8_r8(&regs[HL].high, &regs[DE].high);
      break;
    case 0x63: // LD H, E
      ld_r8_r8(&regs[HL].high, &regs[DE].low);
      break;
    case 0x64: // LD H, H
      ld_r8_r8(&regs[HL].high, &regs[HL].high);
      break;
    case 0x65: // LD H, L
      ld_r8_r8(&regs[HL].high, &regs[HL].low);
      break;
    case 0x66: // LD H, [HL]
      ld_r8_aHL(&regs[HL].high);
      break;
    case 0x67: // LD H, A
      ld_r8_r8(&regs[HL].high, &regs[AF].high);
      break;
    case 0x68: // LD L, B
      ld_r8_r8(&regs[HL].low, &regs[BC].high);
      break;
    case 0x69: // LD L, C
      ld_r8_r8(&regs[HL].low, &regs[BC].low);
      break;
    case 0x6A: // LD L, D
      ld_r8_r8(&regs[HL].low, &regs[DE].high);
      break;
    case 0x6B: // LD L, E
      ld_r8_r8(&regs[HL].low, &regs[DE].low);
      break;
    case 0x6C: // LD L, H
      ld_r8_r8(&regs[HL].low, &regs[HL].high);
      break;
    case 0x6D: // LD L, L
      ld_r8_r8(&regs[HL].low, &regs[HL].low);
      break;
    case 0x6E: // LD L, [HL]
      ld_r8_aHL(&regs[HL].low);
      break;
    case 0x6F: // LD L, A
      ld_r8_r8(&regs[HL].low, &regs[AF].high);
      break;
    case 0x70: // LD [HL], B
      ld_aHL_r8(&regs[BC].high);
      break;
    case 0x71: // LD [HL], C
      ld_aHL_r8(&regs[BC].low);
      break;
    case 0x72: // LD [HL], D
      ld_aHL_r8(&regs[DE].high);
      break;
    case 0x73: // LD [HL], E
      ld_aHL_r8(&regs[DE].low);
      break;
    case 0x74: // LD [HL], H
      ld_aHL_r8(&regs[HL].high);
      break;
    case 0x75: // LD [HL], L
      ld_aHL_r8(&regs[HL].low);
      break;
    case 0x76: // HALT
      // int_halt();
      break;
    case 0x77: // LD [HL], A
      ld_aHL_r8(&regs[AF].high);
      break;
    case 0x78: // LD A, B
      ld_r8_r8(&regs[AF].high, &regs[BC].high);
      break;
    case 0x79: // LD A, C
      ld_r8_r8(&regs[AF].high, &regs[BC].low);
      break;
    case 0x7A: // LD A, D
      ld_r8_r8(&regs[AF].high, &regs[DE].high);
      break;
    case 0x7B: // LD A, E
      ld_r8_r8(&regs[AF].high, &regs[DE].low);
      break;
    case 0x7C: // LD A, H
      ld_r8_r8(&regs[AF].high, &regs[HL].high);
      break;
    case 0x7D: // LD A, L
      ld_r8_r8(&regs[AF].high, &regs[HL].low);
      break;
    case 0x7E: // LD A, [HL]
      ld_r8_aHL(&regs[AF].high);
      break;
    case 0x7F: // LD A, A
      ld_r8_r8(&regs[AF].high, &regs[AF].high);
      break;
    default:
      break;
    }
  }

  fclose(boot_rom);
  fclose(game_rom);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
