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
      case 0x40:
        debug_print(byte, "BIT 0, B");
        bit_u3_r8(0, &regs[BC].high);
        break;
      case 0x41:
        debug_print(byte, "BIT 0, C");
        bit_u3_r8(0, &regs[BC].low);
        break;
      case 0x42:
        debug_print(byte, "BIT 0, D");
        bit_u3_r8(0, &regs[DE].high);
        break;
      case 0x43:
        debug_print(byte, "BIT 0, E");
        bit_u3_r8(0, &regs[DE].low);
        break;
      case 0x44:
        debug_print(byte, "BIT 0, H");
        bit_u3_r8(0, &regs[HL].high);
        break;
      case 0x45:
        debug_print(byte, "BIT 0, L");
        bit_u3_r8(0, &regs[HL].low);
        break;
      case 0x46:
        debug_print(byte, "BIT 0, [HL]");
        bit_u3_aHL(0);
        break;
      case 0x47:
        debug_print(byte, "BIT 0, A");
        bit_u3_r8(0, &regs[AF].high);
        break;
      case 0x48:
        debug_print(byte, "BIT 1, B");
        bit_u3_r8(1, &regs[BC].high);
        break;
      case 0x49:
        debug_print(byte, "BIT 1, C");
        bit_u3_r8(1, &regs[BC].low);
        break;
      case 0x4A:
        debug_print(byte, "BIT 1, D");
        bit_u3_r8(1, &regs[DE].high);
        break;
      case 0x4B:
        debug_print(byte, "BIT 1, E");
        bit_u3_r8(1, &regs[DE].low);
        break;
      case 0x4C:
        debug_print(byte, "BIT 1, H");
        bit_u3_r8(1, &regs[HL].high);
        break;
      case 0x4D:
        debug_print(byte, "BIT 1, L");
        bit_u3_r8(1, &regs[HL].low);
        break;
      case 0x4E:
        debug_print(byte, "BIT 1, [HL]");
        bit_u3_aHL(1);
        break;
      case 0x4F:
        debug_print(byte, "BIT 1, A");
        bit_u3_r8(1, &regs[AF].high);
        break;
      case 0x50:
        debug_print(byte, "BIT 2, B");
        bit_u3_r8(2, &regs[BC].high);
        break;
      case 0x51:
        debug_print(byte, "BIT 2, C");
        bit_u3_r8(2, &regs[BC].low);
        break;
      case 0x52:
        debug_print(byte, "BIT 2, D");
        bit_u3_r8(2, &regs[DE].high);
        break;
      case 0x53:
        debug_print(byte, "BIT 2, E");
        bit_u3_r8(2, &regs[DE].low);
        break;
      case 0x54:
        debug_print(byte, "BIT 2, H");
        bit_u3_r8(2, &regs[HL].high);
        break;
      case 0x55:
        debug_print(byte, "BIT 2, L");
        bit_u3_r8(2, &regs[HL].low);
        break;
      case 0x56:
        debug_print(byte, "BIT 2, [HL]");
        bit_u3_aHL(2);
        break;
      case 0x57:
        debug_print(byte, "BIT 2, A");
        bit_u3_r8(2, &regs[AF].high);
        break;
      case 0x58:
        debug_print(byte, "BIT 3, B");
        bit_u3_r8(3, &regs[BC].high);
        break;
      case 0x59:
        debug_print(byte, "BIT 3, C");
        bit_u3_r8(3, &regs[BC].low);
        break;
      case 0x5A:
        debug_print(byte, "BIT 3, D");
        bit_u3_r8(3, &regs[DE].high);
        break;
      case 0x5B:
        debug_print(byte, "BIT 3, E");
        bit_u3_r8(3, &regs[DE].low);
        break;
      case 0x5C:
        debug_print(byte, "BIT 3, H");
        bit_u3_r8(3, &regs[HL].high);
        break;
      case 0x5D:
        debug_print(byte, "BIT 3, L");
        bit_u3_r8(3, &regs[HL].low);
        break;
      case 0x5E:
        debug_print(byte, "BIT 3, [HL]");
        bit_u3_aHL(3);
        break;
      case 0x5F:
        debug_print(byte, "BIT 3, A");
        bit_u3_r8(3, &regs[AF].high);
        break;
      case 0x60:
        debug_print(byte, "BIT 4, B");
        bit_u3_r8(4, &regs[BC].high);
        break;
      case 0x61:
        debug_print(byte, "BIT 4, C");
        bit_u3_r8(4, &regs[BC].low);
        break;
      case 0x62:
        debug_print(byte, "BIT 4, D");
        bit_u3_r8(4, &regs[DE].high);
        break;
      case 0x63:
        debug_print(byte, "BIT 4, E");
        bit_u3_r8(4, &regs[DE].low);
        break;
      case 0x64:
        debug_print(byte, "BIT 4, H");
        bit_u3_r8(4, &regs[HL].high);
        break;
      case 0x65:
        debug_print(byte, "BIT 4, L");
        bit_u3_r8(4, &regs[HL].low);
        break;
      case 0x66:
        debug_print(byte, "BIT 4, [HL]");
        bit_u3_aHL(4);
        break;
      case 0x67:
        debug_print(byte, "BIT 4, A");
        bit_u3_r8(4, &regs[AF].high);
        break;
      case 0x68:
        debug_print(byte, "BIT 5, B");
        bit_u3_r8(5, &regs[BC].high);
        break;
      case 0x69:
        debug_print(byte, "BIT 5, C");
        bit_u3_r8(5, &regs[BC].low);
        break;
      case 0x6A:
        debug_print(byte, "BIT 5, D");
        bit_u3_r8(5, &regs[DE].high);
        break;
      case 0x6B:
        debug_print(byte, "BIT 5, E");
        bit_u3_r8(5, &regs[DE].low);
        break;
      case 0x6C:
        debug_print(byte, "BIT 5, H");
        bit_u3_r8(5, &regs[HL].high);
        break;
      case 0x6D:
        debug_print(byte, "BIT 5, L");
        bit_u3_r8(5, &regs[HL].low);
        break;
      case 0x6E:
        debug_print(byte, "BIT 5, [HL]");
        bit_u3_aHL(5);
        break;
      case 0x6F:
        debug_print(byte, "BIT 5, A");
        bit_u3_r8(5, &regs[AF].high);
        break;
      case 0x70:
        debug_print(byte, "BIT 6, B");
        bit_u3_r8(6, &regs[BC].high);
        break;
      case 0x71:
        debug_print(byte, "BIT 6, C");
        bit_u3_r8(6, &regs[BC].low);
        break;
      case 0x72:
        debug_print(byte, "BIT 6, D");
        bit_u3_r8(6, &regs[DE].high);
        break;
      case 0x73:
        debug_print(byte, "BIT 6, E");
        bit_u3_r8(6, &regs[DE].low);
        break;
      case 0x74:
        debug_print(byte, "BIT 6, H");
        bit_u3_r8(6, &regs[HL].high);
        break;
      case 0x75:
        debug_print(byte, "BIT 6, L");
        bit_u3_r8(6, &regs[HL].low);
        break;
      case 0x76:
        debug_print(byte, "BIT 6, [HL]");
        bit_u3_aHL(6);
        break;
      case 0x77:
        debug_print(byte, "BIT 6, A");
        bit_u3_r8(6, &regs[AF].high);
        break;
      case 0x78:
        debug_print(byte, "BIT 7, B");
        bit_u3_r8(7, &regs[BC].high);
        break;
      case 0x79:
        debug_print(byte, "BIT 7, C");
        bit_u3_r8(7, &regs[BC].low);
        break;
      case 0x7A:
        debug_print(byte, "BIT 7, D");
        bit_u3_r8(7, &regs[DE].high);
        break;
      case 0x7B:
        debug_print(byte, "BIT 7, E");
        bit_u3_r8(7, &regs[DE].low);
        break;
      case 0x7C:
        debug_print(byte, "BIT 7, H");
        bit_u3_r8(7, &regs[HL].high);
        break;
      case 0x7D:
        debug_print(byte, "BIT 7, L");
        bit_u3_r8(7, &regs[HL].low);
        break;
      case 0x7E:
        debug_print(byte, "BIT 7, [HL]");
        bit_u3_aHL(7);
        break;
      case 0x7F:
        debug_print(byte, "BIT 7, A");
        bit_u3_r8(7, &regs[AF].high);
        break;
      default:
        debug_print(byte, "UNKNOWN PREFIX");
        regs[PC].full += 2;
        cycle += 1;
        break;
      }
      prefix = false;
    } else {
      switch (byte) {
      case 0x00:
        debug_print(byte, "NOP");
        nop();
        break;
      case 0x21:
      {
        debug_print(byte, "LD HL, n16");
        uint16_t n16;
        memcpy(&n16, &game_rom[regs[PC].full + 1], sizeof(n16));
        ld_r16_n16(&regs[HL].full, n16);
      } break;
      case 0x31:
      {
        debug_print(byte, "LD SP, n16");
        uint16_t n16;
        memcpy(&n16, &game_rom[regs[PC].full + 1], sizeof(n16));
        ld_SP_n16(n16);
      } break;
      case 0x32:
        debug_print(byte, "LD HLD, A");
        ld_aHLd_A();
        break;
      case 0x40:
        debug_print(byte, "LD B, B");
        ld_r8_r8(&regs[BC].high, &regs[BC].high);
        break;
      case 0x41:
        debug_print(byte, "LD B, C");
        ld_r8_r8(&regs[BC].high, &regs[BC].low);
        break;
      case 0x42:
        debug_print(byte, "LD B, D");
        ld_r8_r8(&regs[BC].high, &regs[DE].high);
        break;
      case 0x43:
        debug_print(byte, "LD B, E");
        ld_r8_r8(&regs[BC].high, &regs[DE].low);
        break;
      case 0x44:
        debug_print(byte, "LD B, H");
        ld_r8_r8(&regs[BC].high, &regs[HL].high);
        break;
      case 0x45:
        debug_print(byte, "LD B, L");
        ld_r8_r8(&regs[BC].high, &regs[HL].low);
        break;
      case 0x46:
        debug_print(byte, "LD B, [HL]");
        ld_r8_aHL(&regs[BC].high);
        break;
      case 0x47:
        debug_print(byte, "LD B, A");
        ld_r8_r8(&regs[BC].high, &regs[AF].high);
        break;
      case 0x48:
        debug_print(byte, "LD C, B");
        ld_r8_r8(&regs[BC].low, &regs[BC].high);
        break;
      case 0x49:
        debug_print(byte, "LD C, C");
        ld_r8_r8(&regs[BC].low, &regs[BC].low);
        break;
      case 0x4A:
        debug_print(byte, "LD C, D");
        ld_r8_r8(&regs[BC].low, &regs[DE].high);
        break;
      case 0x4B:
        debug_print(byte, "LD C, E");
        ld_r8_r8(&regs[BC].low, &regs[DE].low);
        break;
      case 0x4C:
        debug_print(byte, "LD C, H");
        ld_r8_r8(&regs[BC].low, &regs[HL].high);
        break;
      case 0x4D:
        debug_print(byte, "LD C, L");
        ld_r8_r8(&regs[BC].low, &regs[HL].low);
        break;
      case 0x4E:
        debug_print(byte, "LD C, [HL]");
        ld_r8_aHL(&regs[BC].low);
        break;
      case 0x4F:
        debug_print(byte, "LD C, A");
        ld_r8_r8(&regs[BC].low, &regs[AF].high);
        break;
      case 0x50:
        debug_print(byte, "LD D, B");
        ld_r8_r8(&regs[DE].high, &regs[BC].high);
        break;
      case 0x51:
        debug_print(byte, "LD D, C");
        ld_r8_r8(&regs[DE].high, &regs[BC].low);
        break;
      case 0x52:
        debug_print(byte, "LD D, D");
        ld_r8_r8(&regs[DE].high, &regs[DE].high);
        break;
      case 0x53:
        debug_print(byte, "LD D, E");
        ld_r8_r8(&regs[DE].high, &regs[DE].low);
        break;
      case 0x54:
        debug_print(byte, "LD D, H");
        ld_r8_r8(&regs[DE].high, &regs[HL].high);
        break;
      case 0x55:
        debug_print(byte, "LD D, L");
        ld_r8_r8(&regs[DE].high, &regs[HL].low);
        break;
      case 0x56:
        debug_print(byte, "LD D, [HL]");
        ld_r8_aHL(&regs[DE].high);
        break;
      case 0x57:
        debug_print(byte, "LD D, A");
        ld_r8_r8(&regs[DE].high, &regs[AF].high);
        break;
      case 0x58:
        debug_print(byte, "LD E, B");
        ld_r8_r8(&regs[DE].low, &regs[BC].high);
        break;
      case 0x59:
        debug_print(byte, "LD E, C");
        ld_r8_r8(&regs[DE].low, &regs[BC].low);
        break;
      case 0x5A:
        debug_print(byte, "LD E, D");
        ld_r8_r8(&regs[DE].low, &regs[DE].high);
        break;
      case 0x5B:
        debug_print(byte, "LD E, E");
        ld_r8_r8(&regs[DE].low, &regs[DE].low);
        break;
      case 0x5C:
        debug_print(byte, "LD E, H");
        ld_r8_r8(&regs[DE].low, &regs[HL].high);
        break;
      case 0x5D:
        debug_print(byte, "LD E, L");
        ld_r8_r8(&regs[DE].low, &regs[HL].low);
        break;
      case 0x5E:
        debug_print(byte, "LD E, [HL]");
        ld_r8_aHL(&regs[DE].low);
        break;
      case 0x5F:
        debug_print(byte, "LD E, A");
        ld_r8_r8(&regs[DE].low, &regs[AF].high);
        break;
      case 0x60:
        debug_print(byte, "LD H, B");
        ld_r8_r8(&regs[HL].high, &regs[BC].high);
        break;
      case 0x61:
        debug_print(byte, "LD H, C");
        ld_r8_r8(&regs[HL].high, &regs[BC].low);
        break;
      case 0x62:
        debug_print(byte, "LD H, D");
        ld_r8_r8(&regs[HL].high, &regs[DE].high);
        break;
      case 0x63:
        debug_print(byte, "LD H, E");
        ld_r8_r8(&regs[HL].high, &regs[DE].low);
        break;
      case 0x64:
        debug_print(byte, "LD H, H");
        ld_r8_r8(&regs[HL].high, &regs[HL].high);
        break;
      case 0x65:
        debug_print(byte, "LD H, L");
        ld_r8_r8(&regs[HL].high, &regs[HL].low);
        break;
      case 0x66:
        debug_print(byte, "LD H, [HL]");
        ld_r8_aHL(&regs[HL].high);
        break;
      case 0x67:
        debug_print(byte, "LD H, A");
        ld_r8_r8(&regs[HL].high, &regs[AF].high);
        break;
      case 0x68:
        debug_print(byte, "LD L, B");
        ld_r8_r8(&regs[HL].low, &regs[BC].high);
        break;
      case 0x69:
        debug_print(byte, "LD L, C");
        ld_r8_r8(&regs[HL].low, &regs[BC].low);
        break;
      case 0x6A:
        debug_print(byte, "LD L, D");
        ld_r8_r8(&regs[HL].low, &regs[DE].high);
        break;
      case 0x6B:
        debug_print(byte, "LD L, E");
        ld_r8_r8(&regs[HL].low, &regs[DE].low);
        break;
      case 0x6C:
        debug_print(byte, "LD L, H");
        ld_r8_r8(&regs[HL].low, &regs[HL].high);
        break;
      case 0x6D:
        debug_print(byte, "LD L, L");
        ld_r8_r8(&regs[HL].low, &regs[HL].low);
        break;
      case 0x6E:
        debug_print(byte, "LD L, [HL]");
        ld_r8_aHL(&regs[HL].low);
        break;
      case 0x6F:
        debug_print(byte, "LD L, A");
        ld_r8_r8(&regs[HL].low, &regs[AF].high);
        break;
      case 0x70:
        debug_print(byte, "LD [HL], B");
        ld_aHL_r8(&regs[BC].high);
        break;
      case 0x71:
        debug_print(byte, "LD [HL], C");
        ld_aHL_r8(&regs[BC].low);
        break;
      case 0x72:
        debug_print(byte, "LD [HL], D");
        ld_aHL_r8(&regs[DE].high);
        break;
      case 0x73:
        debug_print(byte, "LD [HL], E");
        ld_aHL_r8(&regs[DE].low);
        break;
      case 0x74:
        debug_print(byte, "LD [HL], H");
        ld_aHL_r8(&regs[HL].high);
        break;
      case 0x75:
        debug_print(byte, "LD [HL], L");
        ld_aHL_r8(&regs[HL].low);
        break;
      case 0x76:
        debug_print(byte, "HALT");
        // int_halt();
        break;
      case 0x77:
        debug_print(byte, "LD [HL], A");
        ld_aHL_r8(&regs[AF].high);
        break;
      case 0x78:
        debug_print(byte, "LD A, B");
        ld_r8_r8(&regs[AF].high, &regs[BC].high);
        break;
      case 0x79:
        debug_print(byte, "LD A, C");
        ld_r8_r8(&regs[AF].high, &regs[BC].low);
        break;
      case 0x7A:
        debug_print(byte, "LD A, D");
        ld_r8_r8(&regs[AF].high, &regs[DE].high);
        break;
      case 0x7B:
        debug_print(byte, "LD A, E");
        ld_r8_r8(&regs[AF].high, &regs[DE].low);
        break;
      case 0x7C:
        debug_print(byte, "LD A, H");
        ld_r8_r8(&regs[AF].high, &regs[HL].high);
        break;
      case 0x7D:
        debug_print(byte, "LD A, L");
        ld_r8_r8(&regs[AF].high, &regs[HL].low);
        break;
      case 0x7E:
        debug_print(byte, "LD A, [HL]");
        ld_r8_aHL(&regs[AF].high);
        break;
      case 0x7F:
        debug_print(byte, "LD A, A");
        ld_r8_r8(&regs[AF].high, &regs[AF].high);
        break;
      case 0xAF:
        debug_print(byte, "XOR A, A");
        xor_A_r8(&regs[AF].high);
        break;
      case 0xCB:
        debug_print(byte, "PREFIX TOGGLE");
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
