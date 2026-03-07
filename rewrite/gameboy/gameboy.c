#include "gameboy.h"
#include "../misc/constants.h"

// since the emulator is also meant to run on STM32, avoid using stdlib headers.
// safe ones are stdint.h, stdbool.h, stddef.h, stdalign.h, and stdnoreturn.h
#include <assert.h>

/**
 * @brief Runs 70224 cycles (one scanline) worth of instructions
 */
void updateState(GameboyState *state) {
	u64 starting_cycle = state->cpu.cycle;
	while (state->cpu.cycle < (starting_cycle + CYCLES_PER_SCANLINE)) {
		assert(starting_cycle <= state->cpu.cycle);
		// TODO: sometimes we might exceed the target cycles - what then?
		// TODO: maybe use modulo?
		// executeOpcode(opcode, state);
	}
}
