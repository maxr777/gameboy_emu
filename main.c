#include "gameboy.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DISP_MULTP 4

// TODO: add debug mode argument
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "No game loaded");
    return 1;
  }

  bool debug = false;

  FILE *game_file = fopen(argv[1], "rb");
  if (!game_file) {
    perror("fopen game fail:");
    return 1;
  }

  // the cartridge header itself goes to 0x014F (inclusive),
  // so if the rom is smaller than 0x0150 it's not a correct ROM
  fseek(game_file, 0, SEEK_END);
  long game_size = ftell(game_file);
  if (game_size < 0x0150) {
    fprintf(stderr, "The ROM is too small");
    return 1;
  }

  uint8_t *game_rom = malloc(game_size);
  if (!game_rom) {
    perror("game rom malloc fail:");
    fclose(game_file);
    return 1;
  }

  fread(game_rom, sizeof(uint8_t), game_size, game_file);
  fclose(game_file);

  // for exact locations: https://gbdev.io/pandocs/The_Cartridge_Header.html
  memcpy(cartridge_header.entry_point, &game_rom[0x0100], sizeof(cartridge_header.entry_point));
  memcpy(cartridge_header.nintendo_logo, &game_rom[0x0104], sizeof(cartridge_header.nintendo_logo));
  memcpy(cartridge_header.title, &game_rom[0x0134], sizeof(cartridge_header.title));
  memcpy(&cartridge_header.cgb_flag, &game_rom[0x0143], sizeof(cartridge_header.cgb_flag));
  memcpy(&cartridge_header.new_license_code, &game_rom[0x0144], sizeof(cartridge_header.new_license_code));
  memcpy(&cartridge_header.sgb_flag, &game_rom[0x0146], sizeof(cartridge_header.sgb_flag));
  memcpy(&cartridge_header.cartridge_type, &game_rom[0x0147], sizeof(cartridge_header.cartridge_type));
  memcpy(&cartridge_header.rom_size, &game_rom[0x0148], sizeof(cartridge_header.rom_size));
  memcpy(&cartridge_header.ram_size, &game_rom[0x0149], sizeof(cartridge_header.ram_size));
  memcpy(&cartridge_header.destination_code, &game_rom[0x014A], sizeof(cartridge_header.destination_code));
  memcpy(&cartridge_header.old_licensee_code, &game_rom[0x014B], sizeof(cartridge_header.old_licensee_code));
  memcpy(&cartridge_header.mask_rom_version_number, &game_rom[0x014C], sizeof(cartridge_header.mask_rom_version_number));
  memcpy(&cartridge_header.header_checksum, &game_rom[0x014D], sizeof(cartridge_header.header_checksum));
  memcpy(&cartridge_header.global_checksum, &game_rom[0x014E], sizeof(cartridge_header.global_checksum));

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
  bool running = true, boot_rom_enabled = true;
  uint8_t byte = 0;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    // the boot rom gets mapped over the game rom until the BOOT_ROM_DISABLE address is 1
    if (io_registers[BOOT_ROM_DISABLE - IO_BASE] == 1)
      boot_rom_enabled = false;

    if (boot_rom_enabled)
      byte = boot_rom[regs[PC].full];
    else
      byte = game_rom[regs[PC].full];

    regs[PC].full += 1;

    switch (byte) {
    case 0x00: // NOP
      if (debug) printf("0x%02X\t%s\n", byte, "NOP");
      ++cycle;
      break;
    // case 0x01: // LD BC, n16
    // {
    //   uint16_t n16;
    //   fread(&n16, 2, 1, game_rom);
    //   if (debug) printf("0x%02X %04X\t%s\n", byte, n16, "LD BC, n16");
    //   ld_r16_n16(&regs[BC].full, n16);
    // } break;
    // case 0x02: // LD [BC], A
    //   ld_a16_A(&regs[BC].full);
    //   break;
    // case 0x03: // INC BC
    //   ++regs[BC].full;
    //   cycle += 2;
    //   break;
    // case 0x04: // INC B
    //   ++regs[BC].high;
    //   ++cycle;
    //   break;
    // case 0x05: // DEC B
    // {
    //   uint8_t old = regs[BC].high;
    //   uint8_t result = --regs[BC].high;
    //   set_half_carry_flag((old & 0x0f) < (result & 0x0f));
    //   set_zero_flag(result == 0);
    //   set_subtraction_flag(true);
    //   ++cycle;
    // } break;
    // case 0x06: // LD B, n8
    //   fread(&regs[BC].high, 1, 1, game_rom);
    //   cycle += 2;
    //   break;
    // case 0x07: // RLCA
    // {
    //   bool result = regs[AF].high & 0x80;
    //   set_carry_flag(result);
    //   regs[AF].high <<= 1;
    //   regs[AF].high = regs[AF].high | result;
    //   set_zero_flag(false);
    //   set_subtraction_flag(false);
    //   set_half_carry_flag(false);
    //   ++cycle;
    // } break;
    // case 0x08: // LD [n16], SP
    // {
    //   uint16_t address;
    //   fread(&address, 2, 1, game_rom);
    //   ld_addr16_SP(address);
    // } break;
    // case 0x09:
    //   break;
    case 0x40: // LD B, B
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, B");
      ld_r8_r8(&regs[BC].high, &regs[BC].high);
      break;
    case 0x41: // LD B, C
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, C");
      ld_r8_r8(&regs[BC].high, &regs[BC].low);
      break;
    case 0x42: // LD B, D
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, D");
      ld_r8_r8(&regs[BC].high, &regs[DE].high);
      break;
    case 0x43: // LD B, E
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, E");
      ld_r8_r8(&regs[BC].high, &regs[DE].low);
      break;
    case 0x44: // LD B, H
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, H");
      ld_r8_r8(&regs[BC].high, &regs[HL].high);
      break;
    case 0x45: // LD B, L
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, L");
      ld_r8_r8(&regs[BC].high, &regs[HL].low);
      break;
    case 0x46: // LD B, [HL]
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, [HL]");
      ld_r8_aHL(&regs[BC].high);
      break;
    case 0x47: // LD B, A
      if (debug) printf("0x%02X\t%s\n", byte, "LD B, A");
      ld_r8_r8(&regs[BC].high, &regs[AF].high);
      break;
    case 0x48: // LD C, B
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, B");
      ld_r8_r8(&regs[BC].low, &regs[BC].high);
      break;
    case 0x49: // LD C, C
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, C");
      ld_r8_r8(&regs[BC].low, &regs[BC].low);
      break;
    case 0x4A: // LD C, D
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, D");
      ld_r8_r8(&regs[BC].low, &regs[DE].high);
      break;
    case 0x4B: // LD C, E
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, E");
      ld_r8_r8(&regs[BC].low, &regs[DE].low);
      break;
    case 0x4C: // LD C, H
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, H");
      ld_r8_r8(&regs[BC].low, &regs[HL].high);
      break;
    case 0x4D: // LD C, L
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, L");
      ld_r8_r8(&regs[BC].low, &regs[HL].low);
      break;
    case 0x4E: // LD C, [HL]
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, [HL]");
      ld_r8_aHL(&regs[BC].low);
      break;
    case 0x4F: // LD C, A
      if (debug) printf("0x%02X\t%s\n", byte, "LD C, A");
      ld_r8_r8(&regs[BC].low, &regs[AF].high);
      break;
    case 0x50: // LD D, B
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, B");
      ld_r8_r8(&regs[DE].high, &regs[BC].high);
      break;
    case 0x51: // LD D, C
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, C");
      ld_r8_r8(&regs[DE].high, &regs[BC].low);
      break;
    case 0x52: // LD D, D
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, D");
      ld_r8_r8(&regs[DE].high, &regs[DE].high);
      break;
    case 0x53: // LD D, E
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, E");
      ld_r8_r8(&regs[DE].high, &regs[DE].low);
      break;
    case 0x54: // LD D, H
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, H");
      ld_r8_r8(&regs[DE].high, &regs[HL].high);
      break;
    case 0x55: // LD D, L
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, L");
      ld_r8_r8(&regs[DE].high, &regs[HL].low);
      break;
    case 0x56: // LD D, [HL]
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, [HL]");
      ld_r8_aHL(&regs[DE].high);
      break;
    case 0x57: // LD D, A
      if (debug) printf("0x%02X\t%s\n", byte, "LD D, A");
      ld_r8_r8(&regs[DE].high, &regs[AF].high);
      break;
    case 0x58: // LD E, B
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, B");
      ld_r8_r8(&regs[DE].low, &regs[BC].high);
      break;
    case 0x59: // LD E, C
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, C");
      ld_r8_r8(&regs[DE].low, &regs[BC].low);
      break;
    case 0x5A: // LD E, D
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, D");
      ld_r8_r8(&regs[DE].low, &regs[DE].high);
      break;
    case 0x5B: // LD E, E
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, E");
      ld_r8_r8(&regs[DE].low, &regs[DE].low);
      break;
    case 0x5C: // LD E, H
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, H");
      ld_r8_r8(&regs[DE].low, &regs[HL].high);
      break;
    case 0x5D: // LD E, L
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, L");
      ld_r8_r8(&regs[DE].low, &regs[HL].low);
      break;
    case 0x5E: // LD E, [HL]
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, [HL]");
      ld_r8_aHL(&regs[DE].low);
      break;
    case 0x5F: // LD E, A
      if (debug) printf("0x%02X\t%s\n", byte, "LD E, A");
      ld_r8_r8(&regs[DE].low, &regs[AF].high);
      break;
    case 0x60: // LD H, B
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, B");
      ld_r8_r8(&regs[HL].high, &regs[BC].high);
      break;
    case 0x61: // LD H, C
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, C");
      ld_r8_r8(&regs[HL].high, &regs[BC].low);
      break;
    case 0x62: // LD H, D
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, D");
      ld_r8_r8(&regs[HL].high, &regs[DE].high);
      break;
    case 0x63: // LD H, E
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, E");
      ld_r8_r8(&regs[HL].high, &regs[DE].low);
      break;
    case 0x64: // LD H, H
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, H");
      ld_r8_r8(&regs[HL].high, &regs[HL].high);
      break;
    case 0x65: // LD H, L
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, L");
      ld_r8_r8(&regs[HL].high, &regs[HL].low);
      break;
    case 0x66: // LD H, [HL]
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, [HL]");
      ld_r8_aHL(&regs[HL].high);
      break;
    case 0x67: // LD H, A
      if (debug) printf("0x%02X\t%s\n", byte, "LD H, A");
      ld_r8_r8(&regs[HL].high, &regs[AF].high);
      break;
    case 0x68: // LD L, B
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, B");
      ld_r8_r8(&regs[HL].low, &regs[BC].high);
      break;
    case 0x69: // LD L, C
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, C");
      ld_r8_r8(&regs[HL].low, &regs[BC].low);
      break;
    case 0x6A: // LD L, D
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, D");
      ld_r8_r8(&regs[HL].low, &regs[DE].high);
      break;
    case 0x6B: // LD L, E
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, E");
      ld_r8_r8(&regs[HL].low, &regs[DE].low);
      break;
    case 0x6C: // LD L, H
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, H");
      ld_r8_r8(&regs[HL].low, &regs[HL].high);
      break;
    case 0x6D: // LD L, L
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, L");
      ld_r8_r8(&regs[HL].low, &regs[HL].low);
      break;
    case 0x6E: // LD L, [HL]
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, [HL]");
      ld_r8_aHL(&regs[HL].low);
      break;
    case 0x6F: // LD L, A
      if (debug) printf("0x%02X\t%s\n", byte, "LD L, A");
      ld_r8_r8(&regs[HL].low, &regs[AF].high);
      break;
    case 0x70: // LD [HL], B
      if (debug) printf("0x%02X\t%s\n", byte, "LD [HL], B");
      ld_aHL_r8(&regs[BC].high);
      break;
    case 0x71: // LD [HL], C
      if (debug) printf("0x%02X\t%s\n", byte, "LD [HL], C");
      ld_aHL_r8(&regs[BC].low);
      break;
    case 0x72: // LD [HL], D
      if (debug) printf("0x%02X\t%s\n", byte, "LD [HL], D");
      ld_aHL_r8(&regs[DE].high);
      break;
    case 0x73: // LD [HL], E
      if (debug) printf("0x%02X\t%s\n", byte, "LD [HL], E");
      ld_aHL_r8(&regs[DE].low);
      break;
    case 0x74: // LD [HL], H
      if (debug) printf("0x%02X\t%s\n", byte, "LD [HL], H");
      ld_aHL_r8(&regs[HL].high);
      break;
    case 0x75: // LD [HL], L
      if (debug) printf("0x%02X\t%s\n", byte, "LD [HL], L");
      ld_aHL_r8(&regs[HL].low);
      break;
    case 0x76: // HALT
      if (debug) printf("0x%02X\t%s\n", byte, "HALT");
      // int_halt();
      break;
    case 0x77: // LD [HL], A
      if (debug) printf("0x%02X\t%s\n", byte, "LD [HL], A");
      ld_aHL_r8(&regs[AF].high);
      break;
    case 0x78: // LD A, B
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, B");
      ld_r8_r8(&regs[AF].high, &regs[BC].high);
      break;
    case 0x79: // LD A, C
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, C");
      ld_r8_r8(&regs[AF].high, &regs[BC].low);
      break;
    case 0x7A: // LD A, D
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, D");
      ld_r8_r8(&regs[AF].high, &regs[DE].high);
      break;
    case 0x7B: // LD A, E
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, E");
      ld_r8_r8(&regs[AF].high, &regs[DE].low);
      break;
    case 0x7C: // LD A, H
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, H");
      ld_r8_r8(&regs[AF].high, &regs[HL].high);
      break;
    case 0x7D: // LD A, L
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, L");
      ld_r8_r8(&regs[AF].high, &regs[HL].low);
      break;
    case 0x7E: // LD A, [HL]
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, [HL]");
      ld_r8_aHL(&regs[AF].high);
      break;
    case 0x7F: // LD A, A
      if (debug) printf("0x%02X\t%s\n", byte, "LD A, A");
      ld_r8_r8(&regs[AF].high, &regs[AF].high);
      break;
    default:
      break;
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
