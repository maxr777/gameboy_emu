#include "gameboy.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DISP_MULTP 4

bool debug = false;

void debug_print(uint8_t opcode, const char *instruction) {
  if (debug) {
    printf("Cycle: %d\tPC: 0x%04X\tOpcode: 0x%02X\t%-12s\tA: %02X\tBC: %04X\tDE: %04X\tHL: %04X\tSP: %04X\tFlags: %c%c%c%c\n",
           cycle, regs[PC].full, opcode, instruction,
           regs[AF].high, regs[BC].full, regs[DE].full, regs[HL].full, regs[SP].full,
           (regs[AF].low & 0x80) ? 'Z' : '-',
           (regs[AF].low & 0x40) ? 'N' : '-',
           (regs[AF].low & 0x20) ? 'H' : '-',
           (regs[AF].low & 0x10) ? 'C' : '-');
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "No game loaded");
    return 1;
  }

  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "-d") == 0)
      debug = true;
  }

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

    if (prefix) {
      switch (byte) {
      default:
        debug_print(byte, "UNKNOWN PREFIX");
        regs[PC].full += 2;
        cycle += 1;
        break;
      }
      prefix = false;
    } else {
      switch (byte) {
      case 0x00: // NOP
        debug_print(byte, "NOP");
        nop();
        break;
      case 0x21: // LD HL, n16
      {
        debug_print(byte, "LD HL, n16");
        uint16_t n16;
        memcpy(&n16, &game_rom[regs[PC].full + 1], sizeof(n16));
        ld_r16_n16(&regs[HL].full, n16);
      } break;
      case 0x31: // LD SP, n16
      {
        debug_print(byte, "LD SP, n16");
        uint16_t n16;
        memcpy(&n16, &game_rom[regs[PC].full + 1], sizeof(n16));
        ld_SP_n16(n16);
      } break;
      case 0x32: // LD HLD, A
        debug_print(byte, "LD HLD, A");
        ld_aHLd_A();
        break;
      case 0x40: // LD B, B
        debug_print(byte, "LD B, B");
        ld_r8_r8(&regs[BC].high, &regs[BC].high);
        break;
      case 0x41: // LD B, C
        debug_print(byte, "LD B, C");
        ld_r8_r8(&regs[BC].high, &regs[BC].low);
        break;
      case 0x42: // LD B, D
        debug_print(byte, "LD B, D");
        ld_r8_r8(&regs[BC].high, &regs[DE].high);
        break;
      case 0x43: // LD B, E
        debug_print(byte, "LD B, E");
        ld_r8_r8(&regs[BC].high, &regs[DE].low);
        break;
      case 0x44: // LD B, H
        debug_print(byte, "LD B, H");
        ld_r8_r8(&regs[BC].high, &regs[HL].high);
        break;
      case 0x45: // LD B, L
        debug_print(byte, "LD B, L");
        ld_r8_r8(&regs[BC].high, &regs[HL].low);
        break;
      case 0x46: // LD B, [HL]
        debug_print(byte, "LD B, [HL]");
        ld_r8_aHL(&regs[BC].high);
        break;
      case 0x47: // LD B, A
        debug_print(byte, "LD B, A");
        ld_r8_r8(&regs[BC].high, &regs[AF].high);
        break;
      case 0x48: // LD C, B
        debug_print(byte, "LD C, B");
        ld_r8_r8(&regs[BC].low, &regs[BC].high);
        break;
      case 0x49: // LD C, C
        debug_print(byte, "LD C, C");
        ld_r8_r8(&regs[BC].low, &regs[BC].low);
        break;
      case 0x4A: // LD C, D
        debug_print(byte, "LD C, D");
        ld_r8_r8(&regs[BC].low, &regs[DE].high);
        break;
      case 0x4B: // LD C, E
        debug_print(byte, "LD C, E");
        ld_r8_r8(&regs[BC].low, &regs[DE].low);
        break;
      case 0x4C: // LD C, H
        debug_print(byte, "LD C, H");
        ld_r8_r8(&regs[BC].low, &regs[HL].high);
        break;
      case 0x4D: // LD C, L
        debug_print(byte, "LD C, L");
        ld_r8_r8(&regs[BC].low, &regs[HL].low);
        break;
      case 0x4E: // LD C, [HL]
        debug_print(byte, "LD C, [HL]");
        ld_r8_aHL(&regs[BC].low);
        break;
      case 0x4F: // LD C, A
        debug_print(byte, "LD C, A");
        ld_r8_r8(&regs[BC].low, &regs[AF].high);
        break;
      case 0x50: // LD D, B
        debug_print(byte, "LD D, B");
        ld_r8_r8(&regs[DE].high, &regs[BC].high);
        break;
      case 0x51: // LD D, C
        debug_print(byte, "LD D, C");
        ld_r8_r8(&regs[DE].high, &regs[BC].low);
        break;
      case 0x52: // LD D, D
        debug_print(byte, "LD D, D");
        ld_r8_r8(&regs[DE].high, &regs[DE].high);
        break;
      case 0x53: // LD D, E
        debug_print(byte, "LD D, E");
        ld_r8_r8(&regs[DE].high, &regs[DE].low);
        break;
      case 0x54: // LD D, H
        debug_print(byte, "LD D, H");
        ld_r8_r8(&regs[DE].high, &regs[HL].high);
        break;
      case 0x55: // LD D, L
        debug_print(byte, "LD D, L");
        ld_r8_r8(&regs[DE].high, &regs[HL].low);
        break;
      case 0x56: // LD D, [HL]
        debug_print(byte, "LD D, [HL]");
        ld_r8_aHL(&regs[DE].high);
        break;
      case 0x57: // LD D, A
        debug_print(byte, "LD D, A");
        ld_r8_r8(&regs[DE].high, &regs[AF].high);
        break;
      case 0x58: // LD E, B
        debug_print(byte, "LD E, B");
        ld_r8_r8(&regs[DE].low, &regs[BC].high);
        break;
      case 0x59: // LD E, C
        debug_print(byte, "LD E, C");
        ld_r8_r8(&regs[DE].low, &regs[BC].low);
        break;
      case 0x5A: // LD E, D
        debug_print(byte, "LD E, D");
        ld_r8_r8(&regs[DE].low, &regs[DE].high);
        break;
      case 0x5B: // LD E, E
        debug_print(byte, "LD E, E");
        ld_r8_r8(&regs[DE].low, &regs[DE].low);
        break;
      case 0x5C: // LD E, H
        debug_print(byte, "LD E, H");
        ld_r8_r8(&regs[DE].low, &regs[HL].high);
        break;
      case 0x5D: // LD E, L
        debug_print(byte, "LD E, L");
        ld_r8_r8(&regs[DE].low, &regs[HL].low);
        break;
      case 0x5E: // LD E, [HL]
        debug_print(byte, "LD E, [HL]");
        ld_r8_aHL(&regs[DE].low);
        break;
      case 0x5F: // LD E, A
        debug_print(byte, "LD E, A");
        ld_r8_r8(&regs[DE].low, &regs[AF].high);
        break;
      case 0x60: // LD H, B
        debug_print(byte, "LD H, B");
        ld_r8_r8(&regs[HL].high, &regs[BC].high);
        break;
      case 0x61: // LD H, C
        debug_print(byte, "LD H, C");
        ld_r8_r8(&regs[HL].high, &regs[BC].low);
        break;
      case 0x62: // LD H, D
        debug_print(byte, "LD H, D");
        ld_r8_r8(&regs[HL].high, &regs[DE].high);
        break;
      case 0x63: // LD H, E
        debug_print(byte, "LD H, E");
        ld_r8_r8(&regs[HL].high, &regs[DE].low);
        break;
      case 0x64: // LD H, H
        debug_print(byte, "LD H, H");
        ld_r8_r8(&regs[HL].high, &regs[HL].high);
        break;
      case 0x65: // LD H, L
        debug_print(byte, "LD H, L");
        ld_r8_r8(&regs[HL].high, &regs[HL].low);
        break;
      case 0x66: // LD H, [HL]
        debug_print(byte, "LD H, [HL]");
        ld_r8_aHL(&regs[HL].high);
        break;
      case 0x67: // LD H, A
        debug_print(byte, "LD H, A");
        ld_r8_r8(&regs[HL].high, &regs[AF].high);
        break;
      case 0x68: // LD L, B
        debug_print(byte, "LD L, B");
        ld_r8_r8(&regs[HL].low, &regs[BC].high);
        break;
      case 0x69: // LD L, C
        debug_print(byte, "LD L, C");
        ld_r8_r8(&regs[HL].low, &regs[BC].low);
        break;
      case 0x6A: // LD L, D
        debug_print(byte, "LD L, D");
        ld_r8_r8(&regs[HL].low, &regs[DE].high);
        break;
      case 0x6B: // LD L, E
        debug_print(byte, "LD L, E");
        ld_r8_r8(&regs[HL].low, &regs[DE].low);
        break;
      case 0x6C: // LD L, H
        debug_print(byte, "LD L, H");
        ld_r8_r8(&regs[HL].low, &regs[HL].high);
        break;
      case 0x6D: // LD L, L
        debug_print(byte, "LD L, L");
        ld_r8_r8(&regs[HL].low, &regs[HL].low);
        break;
      case 0x6E: // LD L, [HL]
        debug_print(byte, "LD L, [HL]");
        ld_r8_aHL(&regs[HL].low);
        break;
      case 0x6F: // LD L, A
        debug_print(byte, "LD L, A");
        ld_r8_r8(&regs[HL].low, &regs[AF].high);
        break;
      case 0x70: // LD [HL], B
        debug_print(byte, "LD [HL], B");
        ld_aHL_r8(&regs[BC].high);
        break;
      case 0x71: // LD [HL], C
        debug_print(byte, "LD [HL], C");
        ld_aHL_r8(&regs[BC].low);
        break;
      case 0x72: // LD [HL], D
        debug_print(byte, "LD [HL], D");
        ld_aHL_r8(&regs[DE].high);
        break;
      case 0x73: // LD [HL], E
        debug_print(byte, "LD [HL], E");
        ld_aHL_r8(&regs[DE].low);
        break;
      case 0x74: // LD [HL], H
        debug_print(byte, "LD [HL], H");
        ld_aHL_r8(&regs[HL].high);
        break;
      case 0x75: // LD [HL], L
        debug_print(byte, "LD [HL], L");
        ld_aHL_r8(&regs[HL].low);
        break;
      case 0x76: // HALT
        debug_print(byte, "HALT");
        // int_halt();
        break;
      case 0x77: // LD [HL], A
        debug_print(byte, "LD [HL], A");
        ld_aHL_r8(&regs[AF].high);
        break;
      case 0x78: // LD A, B
        debug_print(byte, "LD A, B");
        ld_r8_r8(&regs[AF].high, &regs[BC].high);
        break;
      case 0x79: // LD A, C
        debug_print(byte, "LD A, C");
        ld_r8_r8(&regs[AF].high, &regs[BC].low);
        break;
      case 0x7A: // LD A, D
        debug_print(byte, "LD A, D");
        ld_r8_r8(&regs[AF].high, &regs[DE].high);
        break;
      case 0x7B: // LD A, E
        debug_print(byte, "LD A, E");
        ld_r8_r8(&regs[AF].high, &regs[DE].low);
        break;
      case 0x7C: // LD A, H
        debug_print(byte, "LD A, H");
        ld_r8_r8(&regs[AF].high, &regs[HL].high);
        break;
      case 0x7D: // LD A, L
        debug_print(byte, "LD A, L");
        ld_r8_r8(&regs[AF].high, &regs[HL].low);
        break;
      case 0x7E: // LD A, [HL]
        debug_print(byte, "LD A, [HL]");
        ld_r8_aHL(&regs[AF].high);
        break;
      case 0x7F: // LD A, A
        debug_print(byte, "LD A, A");
        ld_r8_r8(&regs[AF].high, &regs[AF].high);
        break;
      case 0xAF: // XOR A, A
        debug_print(byte, "XOR A, A");
        xor_A_r8(&regs[AF].high);
        break;
      case 0xCB: // PREFIX TOGGLE
        prefix = true;
        regs[PC].full += 1;
        break;
      default:
        debug_print(byte, "UNKNOWN");
        regs[PC].full += 1;
        cycle += 1;
        break;
      }
    }
    if (cycle >= 13)
      running = false;
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
