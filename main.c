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
  FILE *game = fopen(argv[1], "rb");

  bool debug = false;

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

  while (running && (fread(&byte, 1, 1, game) == 1)) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }
    switch (byte) {
    case 0x00: // NOP
      if (debug) printf("0x%02X\t%s\n", byte, "NOP");
      ++cycle;
      break;
    case 0x01: // LD BC, n16
    {
      uint16_t n16;
      fread(&n16, 2, 1, game);
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
      fread(&regs[BC].high, 1, 1, game);
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
      fread(&address, 2, 1, game);
      ld_addr16_SP(address);
    } break;
    case 0x09:
      break;
    default:
      break;
    }
  }

  fclose(game);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
