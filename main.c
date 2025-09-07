#include "gameboy.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISP_MULTP 4

bool debug = false;

void debug_print(uint8_t opcode, const char *instruction) {
	if (debug) {
		printf("Cycle: %lu\tPC: 0x%04X\tOpcode: 0x%02X\t%-12s\tA: %02X\t\tBC: %04X\tDE: %04X\tHL: %04X\tSP: %04X\tFlags: %c%c%c%c\n",
			   cpu.cycle, cpu.regs[PC].full, opcode, instruction,
			   cpu.regs[AF].high, cpu.regs[BC].full, cpu.regs[DE].full, cpu.regs[HL].full, cpu.regs[SP].full,
			   (cpu.regs[AF].low & 0x80) ? 'Z' : '-',
			   (cpu.regs[AF].low & 0x40) ? 'N' : '-',
			   (cpu.regs[AF].low & 0x20) ? 'H' : '-',
			   (cpu.regs[AF].low & 0x10) ? 'C' : '-');
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
	rom.game_size = ftell(game_file);
	rewind(game_file);
	if (rom.game_size < 0x0150) {
		fprintf(stderr, "The ROM is too small");
		return 1;
	}

	rom.game_rom = malloc(rom.game_size);
	if (!rom.game_rom) {
		perror("game rom malloc fail:");
		fclose(game_file);
		return 1;
	}

	fread(rom.game_rom, sizeof(uint8_t), rom.game_size, game_file);
	fclose(game_file);

	// for exact locations: https://gbdev.io/pandocs/The_Cartridge_Header.html
	memcpy(rom.cartridge_header.entry_point, &rom.game_rom[0x0100], sizeof(rom.cartridge_header.entry_point));
	memcpy(rom.cartridge_header.nintendo_logo, &rom.game_rom[0x0104], sizeof(rom.cartridge_header.nintendo_logo));
	memcpy(rom.cartridge_header.title, &rom.game_rom[0x0134], sizeof(rom.cartridge_header.title));
	memcpy(&rom.cartridge_header.cgb_flag, &rom.game_rom[0x0143], sizeof(rom.cartridge_header.cgb_flag));
	memcpy(&rom.cartridge_header.new_license_code, &rom.game_rom[0x0144], sizeof(rom.cartridge_header.new_license_code));
	memcpy(&rom.cartridge_header.sgb_flag, &rom.game_rom[0x0146], sizeof(rom.cartridge_header.sgb_flag));
	memcpy(&rom.cartridge_header.cartridge_type, &rom.game_rom[0x0147], sizeof(rom.cartridge_header.cartridge_type));
	memcpy(&rom.cartridge_header.rom_size, &rom.game_rom[0x0148], sizeof(rom.cartridge_header.rom_size));
	memcpy(&rom.cartridge_header.ram_size, &rom.game_rom[0x0149], sizeof(rom.cartridge_header.ram_size));
	memcpy(&rom.cartridge_header.destination_code, &rom.game_rom[0x014A], sizeof(rom.cartridge_header.destination_code));
	memcpy(&rom.cartridge_header.old_licensee_code, &rom.game_rom[0x014B], sizeof(rom.cartridge_header.old_licensee_code));
	memcpy(&rom.cartridge_header.mask_rom_version_number, &rom.game_rom[0x014C], sizeof(rom.cartridge_header.mask_rom_version_number));
	memcpy(&rom.cartridge_header.header_checksum, &rom.game_rom[0x014D], sizeof(rom.cartridge_header.header_checksum));
	memcpy(&rom.cartridge_header.global_checksum, &rom.game_rom[0x014E], sizeof(rom.cartridge_header.global_checksum));

	switch (rom.cartridge_header.rom_size) {
	case 0x00:
		rom.max_rom_banks = 2;
		break;
	case 0x01:
		rom.max_rom_banks = 4;
		break;
	case 0x02:
		rom.max_rom_banks = 8;
		break;
	case 0x03:
		rom.max_rom_banks = 16;
		break;
	case 0x04:
		rom.max_rom_banks = 32;
		break;
	case 0x05:
		rom.max_rom_banks = 64;
		break;
	case 0x06:
		rom.max_rom_banks = 128;
		break;
	case 0x07:
		rom.max_rom_banks = 256;
		break;
	case 0x08:
		rom.max_rom_banks = 512;
		break;
	}

	if (debug) {
		printf("=== Cartridge Header ===\n");
		printf("Entry Point: %02X %02X %02X %02X\n",
			   rom.cartridge_header.entry_point[0], rom.cartridge_header.entry_point[1],
			   rom.cartridge_header.entry_point[2], rom.cartridge_header.entry_point[3]);
		printf("Title: %.16s\n", rom.cartridge_header.title);
		printf("CGB Flag: 0x%02X\n", rom.cartridge_header.cgb_flag);
		printf("New License Code: 0x%04X\n", rom.cartridge_header.new_license_code);
		printf("SGB Flag: 0x%02X\n", rom.cartridge_header.sgb_flag);
		printf("Cartridge Type: 0x%02X\n", rom.cartridge_header.cartridge_type);
		printf("ROM Size: 0x%02X\n", rom.cartridge_header.rom_size);
		printf("RAM Size: 0x%02X\n", rom.cartridge_header.ram_size);
		printf("Destination Code: 0x%02X\n", rom.cartridge_header.destination_code);
		printf("Old Licensee Code: 0x%02X\n", rom.cartridge_header.old_licensee_code);
		printf("Mask ROM Version: 0x%02X\n", rom.cartridge_header.mask_rom_version_number);
		printf("Header Checksum: 0x%02X\n", rom.cartridge_header.header_checksum);
		printf("Global Checksum: 0x%04X\n", rom.cartridge_header.global_checksum);
		printf("========================\n");
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

	// TODO: This is for testing only
	cpu.regs[PC].full = 0x0100;
	rom.boot_rom_enabled = false;
	cpu.regs[AF].full = 0x01B0;
	cpu.regs[BC].full = 0x0013;
	cpu.regs[DE].full = 0x00D8;
	cpu.regs[HL].full = 0x014D;
	cpu.regs[SP].full = 0xFFFE;

	while (running) {
		const uint64_t frame_start_time = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		const uint64_t cycle_checkpoint = cpu.cycle;
		while (cpu.cycle - cycle_checkpoint < CYCLES_PER_FRAME) {

			// the boot rom gets mapped over the game rom until the BOOT_ROM_DISABLE address is 1
			if (io_registers[BOOT_ROM_DISABLE - IO_REGS_ADDR] == 1)
				rom.boot_rom_enabled = false;

			if (rom.boot_rom_enabled)
				byte = rom.boot_rom[cpu.regs[PC].full];
			else
				byte = read8(cpu.regs[PC].full);

			if (cpu.prefix) {
				// ==================== YES-PREFIX ====================
				switch (byte) {
				case 0x00:
					debug_print(byte, "RLC B");
					rlc_r8(&cpu.regs[BC].high);
					break;
				case 0x01:
					debug_print(byte, "RLC C");
					rlc_r8(&cpu.regs[BC].low);
					break;
				case 0x02:
					debug_print(byte, "RLC D");
					rlc_r8(&cpu.regs[DE].high);
					break;
				case 0x03:
					debug_print(byte, "RLC E");
					rlc_r8(&cpu.regs[DE].low);
					break;
				case 0x04:
					debug_print(byte, "RLC H");
					rlc_r8(&cpu.regs[HL].high);
					break;
				case 0x05:
					debug_print(byte, "RLC L");
					rlc_r8(&cpu.regs[HL].low);
					break;
				case 0x06:
					debug_print(byte, "RLC [HL]");
					rlc_aHL();
					break;
				case 0x07:
					debug_print(byte, "RLC A");
					rlc_r8(&cpu.regs[AF].high);
					break;
				case 0x08:
					debug_print(byte, "RRC B");
					rrc_r8(&cpu.regs[BC].high);
					break;
				case 0x09:
					debug_print(byte, "RRC C");
					rrc_r8(&cpu.regs[BC].low);
					break;
				case 0x0A:
					debug_print(byte, "RRC D");
					rrc_r8(&cpu.regs[DE].high);
					break;
				case 0x0B:
					debug_print(byte, "RRC E");
					rrc_r8(&cpu.regs[DE].low);
					break;
				case 0x0C:
					debug_print(byte, "RRC H");
					rrc_r8(&cpu.regs[HL].high);
					break;
				case 0x0D:
					debug_print(byte, "RRC L");
					rrc_r8(&cpu.regs[HL].low);
					break;
				case 0x0E:
					debug_print(byte, "RRC [HL]");
					rrc_aHL();
					break;
				case 0x0F:
					debug_print(byte, "RRC A");
					rrc_r8(&cpu.regs[AF].high);
					break;
				case 0x10:
					debug_print(byte, "RL B");
					rl_r8(&cpu.regs[BC].high);
					break;
				case 0x11:
					debug_print(byte, "RL C");
					rl_r8(&cpu.regs[BC].low);
					break;
				case 0x12:
					debug_print(byte, "RL D");
					rl_r8(&cpu.regs[DE].high);
					break;
				case 0x13:
					debug_print(byte, "RL E");
					rl_r8(&cpu.regs[DE].low);
					break;
				case 0x14:
					debug_print(byte, "RL H");
					rl_r8(&cpu.regs[HL].high);
					break;
				case 0x15:
					debug_print(byte, "RL L");
					rl_r8(&cpu.regs[HL].low);
					break;
				case 0x16:
					debug_print(byte, "RL [HL]");
					rl_aHL();
					break;
				case 0x17:
					debug_print(byte, "RL A");
					rl_r8(&cpu.regs[AF].high);
					break;
				case 0x18:
					debug_print(byte, "RR B");
					rr_r8(&cpu.regs[BC].high);
					break;
				case 0x19:
					debug_print(byte, "RR C");
					rr_r8(&cpu.regs[BC].low);
					break;
				case 0x1A:
					debug_print(byte, "RR D");
					rr_r8(&cpu.regs[DE].high);
					break;
				case 0x1B:
					debug_print(byte, "RR E");
					rr_r8(&cpu.regs[DE].low);
					break;
				case 0x1C:
					debug_print(byte, "RR H");
					rr_r8(&cpu.regs[HL].high);
					break;
				case 0x1D:
					debug_print(byte, "RR L");
					rr_r8(&cpu.regs[HL].low);
					break;
				case 0x1E:
					debug_print(byte, "RR [HL]");
					rr_aHL();
					break;
				case 0x1F:
					debug_print(byte, "RR A");
					rr_r8(&cpu.regs[AF].high);
					break;
				case 0x20:
					debug_print(byte, "SLA B");
					sla_r8(&cpu.regs[BC].high);
					break;
				case 0x21:
					debug_print(byte, "SLA C");
					sla_r8(&cpu.regs[BC].low);
					break;
				case 0x22:
					debug_print(byte, "SLA D");
					sla_r8(&cpu.regs[DE].high);
					break;
				case 0x23:
					debug_print(byte, "SLA E");
					sla_r8(&cpu.regs[DE].low);
					break;
				case 0x24:
					debug_print(byte, "SLA H");
					sla_r8(&cpu.regs[HL].high);
					break;
				case 0x25:
					debug_print(byte, "SLA L");
					sla_r8(&cpu.regs[HL].low);
					break;
				case 0x26:
					debug_print(byte, "SLA [HL]");
					sla_aHL();
					break;
				case 0x27:
					debug_print(byte, "SLA A");
					sla_r8(&cpu.regs[AF].high);
					break;
				case 0x28:
					debug_print(byte, "SRA B");
					sra_r8(&cpu.regs[BC].high);
					break;
				case 0x29:
					debug_print(byte, "SRA C");
					sra_r8(&cpu.regs[BC].low);
					break;
				case 0x2A:
					debug_print(byte, "SRA D");
					sra_r8(&cpu.regs[DE].high);
					break;
				case 0x2B:
					debug_print(byte, "SRA E");
					sra_r8(&cpu.regs[DE].low);
					break;
				case 0x2C:
					debug_print(byte, "SRA H");
					sra_r8(&cpu.regs[HL].high);
					break;
				case 0x2D:
					debug_print(byte, "SRA L");
					sra_r8(&cpu.regs[HL].low);
					break;
				case 0x2E:
					debug_print(byte, "SRA [HL]");
					sra_aHL();
					break;
				case 0x2F:
					debug_print(byte, "SRA A");
					sra_r8(&cpu.regs[AF].high);
					break;
				case 0x30:
					debug_print(byte, "SWAP B");
					swap_r8(&cpu.regs[BC].high);
					break;
				case 0x31:
					debug_print(byte, "SWAP C");
					swap_r8(&cpu.regs[BC].low);
					break;
				case 0x32:
					debug_print(byte, "SWAP D");
					swap_r8(&cpu.regs[DE].high);
					break;
				case 0x33:
					debug_print(byte, "SWAP E");
					swap_r8(&cpu.regs[DE].low);
					break;
				case 0x34:
					debug_print(byte, "SWAP H");
					swap_r8(&cpu.regs[HL].high);
					break;
				case 0x35:
					debug_print(byte, "SWAP L");
					swap_r8(&cpu.regs[HL].low);
					break;
				case 0x36:
					debug_print(byte, "SWAP [HL]");
					swap_aHL();
					break;
				case 0x37:
					debug_print(byte, "SWAP A");
					swap_r8(&cpu.regs[AF].high);
					break;
				case 0x38:
					debug_print(byte, "SRL B");
					srl_r8(&cpu.regs[BC].high);
					break;
				case 0x39:
					debug_print(byte, "SRL C");
					srl_r8(&cpu.regs[BC].low);
					break;
				case 0x3A:
					debug_print(byte, "SRL D");
					srl_r8(&cpu.regs[DE].high);
					break;
				case 0x3B:
					debug_print(byte, "SRL E");
					srl_r8(&cpu.regs[DE].low);
					break;
				case 0x3C:
					debug_print(byte, "SRL H");
					srl_r8(&cpu.regs[HL].high);
					break;
				case 0x3D:
					debug_print(byte, "SRL L");
					srl_r8(&cpu.regs[HL].low);
					break;
				case 0x3E:
					debug_print(byte, "SRL [HL]");
					srl_aHL();
					break;
				case 0x3F:
					debug_print(byte, "SRL A");
					srl_r8(&cpu.regs[AF].high);
					break;
				case 0x40:
					debug_print(byte, "BIT 0, B");
					bit_u3_r8(0, cpu.regs[BC].high);
					break;
				case 0x41:
					debug_print(byte, "BIT 0, C");
					bit_u3_r8(0, cpu.regs[BC].low);
					break;
				case 0x42:
					debug_print(byte, "BIT 0, D");
					bit_u3_r8(0, cpu.regs[DE].high);
					break;
				case 0x43:
					debug_print(byte, "BIT 0, E");
					bit_u3_r8(0, cpu.regs[DE].low);
					break;
				case 0x44:
					debug_print(byte, "BIT 0, H");
					bit_u3_r8(0, cpu.regs[HL].high);
					break;
				case 0x45:
					debug_print(byte, "BIT 0, L");
					bit_u3_r8(0, cpu.regs[HL].low);
					break;
				case 0x46:
					debug_print(byte, "BIT 0, [HL]");
					bit_u3_aHL(0);
					break;
				case 0x47:
					debug_print(byte, "BIT 0, A");
					bit_u3_r8(0, cpu.regs[AF].high);
					break;
				case 0x48:
					debug_print(byte, "BIT 1, B");
					bit_u3_r8(1, cpu.regs[BC].high);
					break;
				case 0x49:
					debug_print(byte, "BIT 1, C");
					bit_u3_r8(1, cpu.regs[BC].low);
					break;
				case 0x4A:
					debug_print(byte, "BIT 1, D");
					bit_u3_r8(1, cpu.regs[DE].high);
					break;
				case 0x4B:
					debug_print(byte, "BIT 1, E");
					bit_u3_r8(1, cpu.regs[DE].low);
					break;
				case 0x4C:
					debug_print(byte, "BIT 1, H");
					bit_u3_r8(1, cpu.regs[HL].high);
					break;
				case 0x4D:
					debug_print(byte, "BIT 1, L");
					bit_u3_r8(1, cpu.regs[HL].low);
					break;
				case 0x4E:
					debug_print(byte, "BIT 1, [HL]");
					bit_u3_aHL(1);
					break;
				case 0x4F:
					debug_print(byte, "BIT 1, A");
					bit_u3_r8(1, cpu.regs[AF].high);
					break;
				case 0x50:
					debug_print(byte, "BIT 2, B");
					bit_u3_r8(2, cpu.regs[BC].high);
					break;
				case 0x51:
					debug_print(byte, "BIT 2, C");
					bit_u3_r8(2, cpu.regs[BC].low);
					break;
				case 0x52:
					debug_print(byte, "BIT 2, D");
					bit_u3_r8(2, cpu.regs[DE].high);
					break;
				case 0x53:
					debug_print(byte, "BIT 2, E");
					bit_u3_r8(2, cpu.regs[DE].low);
					break;
				case 0x54:
					debug_print(byte, "BIT 2, H");
					bit_u3_r8(2, cpu.regs[HL].high);
					break;
				case 0x55:
					debug_print(byte, "BIT 2, L");
					bit_u3_r8(2, cpu.regs[HL].low);
					break;
				case 0x56:
					debug_print(byte, "BIT 2, [HL]");
					bit_u3_aHL(2);
					break;
				case 0x57:
					debug_print(byte, "BIT 2, A");
					bit_u3_r8(2, cpu.regs[AF].high);
					break;
				case 0x58:
					debug_print(byte, "BIT 3, B");
					bit_u3_r8(3, cpu.regs[BC].high);
					break;
				case 0x59:
					debug_print(byte, "BIT 3, C");
					bit_u3_r8(3, cpu.regs[BC].low);
					break;
				case 0x5A:
					debug_print(byte, "BIT 3, D");
					bit_u3_r8(3, cpu.regs[DE].high);
					break;
				case 0x5B:
					debug_print(byte, "BIT 3, E");
					bit_u3_r8(3, cpu.regs[DE].low);
					break;
				case 0x5C:
					debug_print(byte, "BIT 3, H");
					bit_u3_r8(3, cpu.regs[HL].high);
					break;
				case 0x5D:
					debug_print(byte, "BIT 3, L");
					bit_u3_r8(3, cpu.regs[HL].low);
					break;
				case 0x5E:
					debug_print(byte, "BIT 3, [HL]");
					bit_u3_aHL(3);
					break;
				case 0x5F:
					debug_print(byte, "BIT 3, A");
					bit_u3_r8(3, cpu.regs[AF].high);
					break;
				case 0x60:
					debug_print(byte, "BIT 4, B");
					bit_u3_r8(4, cpu.regs[BC].high);
					break;
				case 0x61:
					debug_print(byte, "BIT 4, C");
					bit_u3_r8(4, cpu.regs[BC].low);
					break;
				case 0x62:
					debug_print(byte, "BIT 4, D");
					bit_u3_r8(4, cpu.regs[DE].high);
					break;
				case 0x63:
					debug_print(byte, "BIT 4, E");
					bit_u3_r8(4, cpu.regs[DE].low);
					break;
				case 0x64:
					debug_print(byte, "BIT 4, H");
					bit_u3_r8(4, cpu.regs[HL].high);
					break;
				case 0x65:
					debug_print(byte, "BIT 4, L");
					bit_u3_r8(4, cpu.regs[HL].low);
					break;
				case 0x66:
					debug_print(byte, "BIT 4, [HL]");
					bit_u3_aHL(4);
					break;
				case 0x67:
					debug_print(byte, "BIT 4, A");
					bit_u3_r8(4, cpu.regs[AF].high);
					break;
				case 0x68:
					debug_print(byte, "BIT 5, B");
					bit_u3_r8(5, cpu.regs[BC].high);
					break;
				case 0x69:
					debug_print(byte, "BIT 5, C");
					bit_u3_r8(5, cpu.regs[BC].low);
					break;
				case 0x6A:
					debug_print(byte, "BIT 5, D");
					bit_u3_r8(5, cpu.regs[DE].high);
					break;
				case 0x6B:
					debug_print(byte, "BIT 5, E");
					bit_u3_r8(5, cpu.regs[DE].low);
					break;
				case 0x6C:
					debug_print(byte, "BIT 5, H");
					bit_u3_r8(5, cpu.regs[HL].high);
					break;
				case 0x6D:
					debug_print(byte, "BIT 5, L");
					bit_u3_r8(5, cpu.regs[HL].low);
					break;
				case 0x6E:
					debug_print(byte, "BIT 5, [HL]");
					bit_u3_aHL(5);
					break;
				case 0x6F:
					debug_print(byte, "BIT 5, A");
					bit_u3_r8(5, cpu.regs[AF].high);
					break;
				case 0x70:
					debug_print(byte, "BIT 6, B");
					bit_u3_r8(6, cpu.regs[BC].high);
					break;
				case 0x71:
					debug_print(byte, "BIT 6, C");
					bit_u3_r8(6, cpu.regs[BC].low);
					break;
				case 0x72:
					debug_print(byte, "BIT 6, D");
					bit_u3_r8(6, cpu.regs[DE].high);
					break;
				case 0x73:
					debug_print(byte, "BIT 6, E");
					bit_u3_r8(6, cpu.regs[DE].low);
					break;
				case 0x74:
					debug_print(byte, "BIT 6, H");
					bit_u3_r8(6, cpu.regs[HL].high);
					break;
				case 0x75:
					debug_print(byte, "BIT 6, L");
					bit_u3_r8(6, cpu.regs[HL].low);
					break;
				case 0x76:
					debug_print(byte, "BIT 6, [HL]");
					bit_u3_aHL(6);
					break;
				case 0x77:
					debug_print(byte, "BIT 6, A");
					bit_u3_r8(6, cpu.regs[AF].high);
					break;
				case 0x78:
					debug_print(byte, "BIT 7, B");
					bit_u3_r8(7, cpu.regs[BC].high);
					break;
				case 0x79:
					debug_print(byte, "BIT 7, C");
					bit_u3_r8(7, cpu.regs[BC].low);
					break;
				case 0x7A:
					debug_print(byte, "BIT 7, D");
					bit_u3_r8(7, cpu.regs[DE].high);
					break;
				case 0x7B:
					debug_print(byte, "BIT 7, E");
					bit_u3_r8(7, cpu.regs[DE].low);
					break;
				case 0x7C:
					debug_print(byte, "BIT 7, H");
					bit_u3_r8(7, cpu.regs[HL].high);
					break;
				case 0x7D:
					debug_print(byte, "BIT 7, L");
					bit_u3_r8(7, cpu.regs[HL].low);
					break;
				case 0x7E:
					debug_print(byte, "BIT 7, [HL]");
					bit_u3_aHL(7);
					break;
				case 0x7F:
					debug_print(byte, "BIT 7, A");
					bit_u3_r8(7, cpu.regs[AF].high);
					break;
				case 0x80:
					debug_print(byte, "RES 0, B");
					res_u3_r8(0, &cpu.regs[BC].high);
					break;
				case 0x81:
					debug_print(byte, "RES 0, C");
					res_u3_r8(0, &cpu.regs[BC].low);
					break;
				case 0x82:
					debug_print(byte, "RES 0, D");
					res_u3_r8(0, &cpu.regs[DE].high);
					break;
				case 0x83:
					debug_print(byte, "RES 0, E");
					res_u3_r8(0, &cpu.regs[DE].low);
					break;
				case 0x84:
					debug_print(byte, "RES 0, H");
					res_u3_r8(0, &cpu.regs[HL].high);
					break;
				case 0x85:
					debug_print(byte, "RES 0, L");
					res_u3_r8(0, &cpu.regs[HL].low);
					break;
				case 0x86:
					debug_print(byte, "RES 0, [HL]");
					res_u3_aHL(0);
					break;
				case 0x87:
					debug_print(byte, "RES 0, A");
					res_u3_r8(0, &cpu.regs[AF].high);
					break;
				case 0x88:
					debug_print(byte, "RES 1, B");
					res_u3_r8(1, &cpu.regs[BC].high);
					break;
				case 0x89:
					debug_print(byte, "RES 1, C");
					res_u3_r8(1, &cpu.regs[BC].low);
					break;
				case 0x8A:
					debug_print(byte, "RES 1, D");
					res_u3_r8(1, &cpu.regs[DE].high);
					break;
				case 0x8B:
					debug_print(byte, "RES 1, E");
					res_u3_r8(1, &cpu.regs[DE].low);
					break;
				case 0x8C:
					debug_print(byte, "RES 1, H");
					res_u3_r8(1, &cpu.regs[HL].high);
					break;
				case 0x8D:
					debug_print(byte, "RES 1, L");
					res_u3_r8(1, &cpu.regs[HL].low);
					break;
				case 0x8E:
					debug_print(byte, "RES 1, [HL]");
					res_u3_aHL(1);
					break;
				case 0x8F:
					debug_print(byte, "RES 1, A");
					res_u3_r8(1, &cpu.regs[AF].high);
					break;
				case 0x90:
					debug_print(byte, "RES 2, B");
					res_u3_r8(2, &cpu.regs[BC].high);
					break;
				case 0x91:
					debug_print(byte, "RES 2, C");
					res_u3_r8(2, &cpu.regs[BC].low);
					break;
				case 0x92:
					debug_print(byte, "RES 2, D");
					res_u3_r8(2, &cpu.regs[DE].high);
					break;
				case 0x93:
					debug_print(byte, "RES 2, E");
					res_u3_r8(2, &cpu.regs[DE].low);
					break;
				case 0x94:
					debug_print(byte, "RES 2, H");
					res_u3_r8(2, &cpu.regs[HL].high);
					break;
				case 0x95:
					debug_print(byte, "RES 2, L");
					res_u3_r8(2, &cpu.regs[HL].low);
					break;
				case 0x96:
					debug_print(byte, "RES 2, [HL]");
					res_u3_aHL(2);
					break;
				case 0x97:
					debug_print(byte, "RES 2, A");
					res_u3_r8(2, &cpu.regs[AF].high);
					break;
				case 0x98:
					debug_print(byte, "RES 3, B");
					res_u3_r8(3, &cpu.regs[BC].high);
					break;
				case 0x99:
					debug_print(byte, "RES 3, C");
					res_u3_r8(3, &cpu.regs[BC].low);
					break;
				case 0x9A:
					debug_print(byte, "RES 3, D");
					res_u3_r8(3, &cpu.regs[DE].high);
					break;
				case 0x9B:
					debug_print(byte, "RES 3, E");
					res_u3_r8(3, &cpu.regs[DE].low);
					break;
				case 0x9C:
					debug_print(byte, "RES 3, H");
					res_u3_r8(3, &cpu.regs[HL].high);
					break;
				case 0x9D:
					debug_print(byte, "RES 3, L");
					res_u3_r8(3, &cpu.regs[HL].low);
					break;
				case 0x9E:
					debug_print(byte, "RES 3, [HL]");
					res_u3_aHL(3);
					break;
				case 0x9F:
					debug_print(byte, "RES 3, A");
					res_u3_r8(3, &cpu.regs[AF].high);
					break;
				case 0xA0:
					debug_print(byte, "RES 4, B");
					res_u3_r8(4, &cpu.regs[BC].high);
					break;
				case 0xA1:
					debug_print(byte, "RES 4, C");
					res_u3_r8(4, &cpu.regs[BC].low);
					break;
				case 0xA2:
					debug_print(byte, "RES 4, D");
					res_u3_r8(4, &cpu.regs[DE].high);
					break;
				case 0xA3:
					debug_print(byte, "RES 4, E");
					res_u3_r8(4, &cpu.regs[DE].low);
					break;
				case 0xA4:
					debug_print(byte, "RES 4, H");
					res_u3_r8(4, &cpu.regs[HL].high);
					break;
				case 0xA5:
					debug_print(byte, "RES 4, L");
					res_u3_r8(4, &cpu.regs[HL].low);
					break;
				case 0xA6:
					debug_print(byte, "RES 4, [HL]");
					res_u3_aHL(4);
					break;
				case 0xA7:
					debug_print(byte, "RES 4, A");
					res_u3_r8(4, &cpu.regs[AF].high);
					break;
				case 0xA8:
					debug_print(byte, "RES 5, B");
					res_u3_r8(5, &cpu.regs[BC].high);
					break;
				case 0xA9:
					debug_print(byte, "RES 5, C");
					res_u3_r8(5, &cpu.regs[BC].low);
					break;
				case 0xAA:
					debug_print(byte, "RES 5, D");
					res_u3_r8(5, &cpu.regs[DE].high);
					break;
				case 0xAB:
					debug_print(byte, "RES 5, E");
					res_u3_r8(5, &cpu.regs[DE].low);
					break;
				case 0xAC:
					debug_print(byte, "RES 5, H");
					res_u3_r8(5, &cpu.regs[HL].high);
					break;
				case 0xAD:
					debug_print(byte, "RES 5, L");
					res_u3_r8(5, &cpu.regs[HL].low);
					break;
				case 0xAE:
					debug_print(byte, "RES 5, [HL]");
					res_u3_aHL(5);
					break;
				case 0xAF:
					debug_print(byte, "RES 5, A");
					res_u3_r8(5, &cpu.regs[AF].high);
					break;
				case 0xB0:
					debug_print(byte, "RES 6, B");
					res_u3_r8(6, &cpu.regs[BC].high);
					break;
				case 0xB1:
					debug_print(byte, "RES 6, C");
					res_u3_r8(6, &cpu.regs[BC].low);
					break;
				case 0xB2:
					debug_print(byte, "RES 6, D");
					res_u3_r8(6, &cpu.regs[DE].high);
					break;
				case 0xB3:
					debug_print(byte, "RES 6, E");
					res_u3_r8(6, &cpu.regs[DE].low);
					break;
				case 0xB4:
					debug_print(byte, "RES 6, H");
					res_u3_r8(6, &cpu.regs[HL].high);
					break;
				case 0xB5:
					debug_print(byte, "RES 6, L");
					res_u3_r8(6, &cpu.regs[HL].low);
					break;
				case 0xB6:
					debug_print(byte, "RES 6, [HL]");
					res_u3_aHL(6);
					break;
				case 0xB7:
					debug_print(byte, "RES 6, A");
					res_u3_r8(6, &cpu.regs[AF].high);
					break;
				case 0xB8:
					debug_print(byte, "RES 7, B");
					res_u3_r8(7, &cpu.regs[BC].high);
					break;
				case 0xB9:
					debug_print(byte, "RES 7, C");
					res_u3_r8(7, &cpu.regs[BC].low);
					break;
				case 0xBA:
					debug_print(byte, "RES 7, D");
					res_u3_r8(7, &cpu.regs[DE].high);
					break;
				case 0xBB:
					debug_print(byte, "RES 7, E");
					res_u3_r8(7, &cpu.regs[DE].low);
					break;
				case 0xBC:
					debug_print(byte, "RES 7, H");
					res_u3_r8(7, &cpu.regs[HL].high);
					break;
				case 0xBD:
					debug_print(byte, "RES 7, L");
					res_u3_r8(7, &cpu.regs[HL].low);
					break;
				case 0xBE:
					debug_print(byte, "RES 7, [HL]");
					res_u3_aHL(7);
					break;
				case 0xBF:
					debug_print(byte, "RES 7, A");
					res_u3_r8(7, &cpu.regs[AF].high);
					break;
				case 0xC0:
					debug_print(byte, "SET 0, B");
					set_u3_r8(0, &cpu.regs[BC].high);
					break;
				case 0xC1:
					debug_print(byte, "SET 0, C");
					set_u3_r8(0, &cpu.regs[BC].low);
					break;
				case 0xC2:
					debug_print(byte, "SET 0, D");
					set_u3_r8(0, &cpu.regs[DE].high);
					break;
				case 0xC3:
					debug_print(byte, "SET 0, E");
					set_u3_r8(0, &cpu.regs[DE].low);
					break;
				case 0xC4:
					debug_print(byte, "SET 0, H");
					set_u3_r8(0, &cpu.regs[HL].high);
					break;
				case 0xC5:
					debug_print(byte, "SET 0, L");
					set_u3_r8(0, &cpu.regs[HL].low);
					break;
				case 0xC6:
					debug_print(byte, "SET 0, [HL]");
					set_u3_aHL(0);
					break;
				case 0xC7:
					debug_print(byte, "SET 0, A");
					set_u3_r8(0, &cpu.regs[AF].high);
					break;
				case 0xC8:
					debug_print(byte, "SET 1, B");
					set_u3_r8(1, &cpu.regs[BC].high);
					break;
				case 0xC9:
					debug_print(byte, "SET 1, C");
					set_u3_r8(1, &cpu.regs[BC].low);
					break;
				case 0xCA:
					debug_print(byte, "SET 1, D");
					set_u3_r8(1, &cpu.regs[DE].high);
					break;
				case 0xCB:
					debug_print(byte, "SET 1, E");
					set_u3_r8(1, &cpu.regs[DE].low);
					break;
				case 0xCC:
					debug_print(byte, "SET 1, H");
					set_u3_r8(1, &cpu.regs[HL].high);
					break;
				case 0xCD:
					debug_print(byte, "SET 1, L");
					set_u3_r8(1, &cpu.regs[HL].low);
					break;
				case 0xCE:
					debug_print(byte, "SET 1, [HL]");
					set_u3_aHL(1);
					break;
				case 0xCF:
					debug_print(byte, "SET 1, A");
					set_u3_r8(1, &cpu.regs[AF].high);
					break;
				case 0xD0:
					debug_print(byte, "SET 2, B");
					set_u3_r8(2, &cpu.regs[BC].high);
					break;
				case 0xD1:
					debug_print(byte, "SET 2, C");
					set_u3_r8(2, &cpu.regs[BC].low);
					break;
				case 0xD2:
					debug_print(byte, "SET 2, D");
					set_u3_r8(2, &cpu.regs[DE].high);
					break;
				case 0xD3:
					debug_print(byte, "SET 2, E");
					set_u3_r8(2, &cpu.regs[DE].low);
					break;
				case 0xD4:
					debug_print(byte, "SET 2, H");
					set_u3_r8(2, &cpu.regs[HL].high);
					break;
				case 0xD5:
					debug_print(byte, "SET 2, L");
					set_u3_r8(2, &cpu.regs[HL].low);
					break;
				case 0xD6:
					debug_print(byte, "SET 2, [HL]");
					set_u3_aHL(2);
					break;
				case 0xD7:
					debug_print(byte, "SET 2, A");
					set_u3_r8(2, &cpu.regs[AF].high);
					break;
				case 0xD8:
					debug_print(byte, "SET 3, B");
					set_u3_r8(3, &cpu.regs[BC].high);
					break;
				case 0xD9:
					debug_print(byte, "SET 3, C");
					set_u3_r8(3, &cpu.regs[BC].low);
					break;
				case 0xDA:
					debug_print(byte, "SET 3, D");
					set_u3_r8(3, &cpu.regs[DE].high);
					break;
				case 0xDB:
					debug_print(byte, "SET 3, E");
					set_u3_r8(3, &cpu.regs[DE].low);
					break;
				case 0xDC:
					debug_print(byte, "SET 3, H");
					set_u3_r8(3, &cpu.regs[HL].high);
					break;
				case 0xDD:
					debug_print(byte, "SET 3, L");
					set_u3_r8(3, &cpu.regs[HL].low);
					break;
				case 0xDE:
					debug_print(byte, "SET 3, [HL]");
					set_u3_aHL(3);
					break;
				case 0xDF:
					debug_print(byte, "SET 3, A");
					set_u3_r8(3, &cpu.regs[AF].high);
					break;
				case 0xE0:
					debug_print(byte, "SET 4, B");
					set_u3_r8(4, &cpu.regs[BC].high);
					break;
				case 0xE1:
					debug_print(byte, "SET 4, C");
					set_u3_r8(4, &cpu.regs[BC].low);
					break;
				case 0xE2:
					debug_print(byte, "SET 4, D");
					set_u3_r8(4, &cpu.regs[DE].high);
					break;
				case 0xE3:
					debug_print(byte, "SET 4, E");
					set_u3_r8(4, &cpu.regs[DE].low);
					break;
				case 0xE4:
					debug_print(byte, "SET 4, H");
					set_u3_r8(4, &cpu.regs[HL].high);
					break;
				case 0xE5:
					debug_print(byte, "SET 4, L");
					set_u3_r8(4, &cpu.regs[HL].low);
					break;
				case 0xE6:
					debug_print(byte, "SET 4, [HL]");
					set_u3_aHL(4);
					break;
				case 0xE7:
					debug_print(byte, "SET 4, A");
					set_u3_r8(4, &cpu.regs[AF].high);
					break;
				case 0xE8:
					debug_print(byte, "SET 5, B");
					set_u3_r8(5, &cpu.regs[BC].high);
					break;
				case 0xE9:
					debug_print(byte, "SET 5, C");
					set_u3_r8(5, &cpu.regs[BC].low);
					break;
				case 0xEA:
					debug_print(byte, "SET 5, D");
					set_u3_r8(5, &cpu.regs[DE].high);
					break;
				case 0xEB:
					debug_print(byte, "SET 5, E");
					set_u3_r8(5, &cpu.regs[DE].low);
					break;
				case 0xEC:
					debug_print(byte, "SET 5, H");
					set_u3_r8(5, &cpu.regs[HL].high);
					break;
				case 0xED:
					debug_print(byte, "SET 5, L");
					set_u3_r8(5, &cpu.regs[HL].low);
					break;
				case 0xEE:
					debug_print(byte, "SET 5, [HL]");
					set_u3_aHL(5);
					break;
				case 0xEF:
					debug_print(byte, "SET 5, A");
					set_u3_r8(5, &cpu.regs[AF].high);
					break;
				case 0xF0:
					debug_print(byte, "SET 6, B");
					set_u3_r8(6, &cpu.regs[BC].high);
					break;
				case 0xF1:
					debug_print(byte, "SET 6, C");
					set_u3_r8(6, &cpu.regs[BC].low);
					break;
				case 0xF2:
					debug_print(byte, "SET 6, D");
					set_u3_r8(6, &cpu.regs[DE].high);
					break;
				case 0xF3:
					debug_print(byte, "SET 6, E");
					set_u3_r8(6, &cpu.regs[DE].low);
					break;
				case 0xF4:
					debug_print(byte, "SET 6, H");
					set_u3_r8(6, &cpu.regs[HL].high);
					break;
				case 0xF5:
					debug_print(byte, "SET 6, L");
					set_u3_r8(6, &cpu.regs[HL].low);
					break;
				case 0xF6:
					debug_print(byte, "SET 6, [HL]");
					set_u3_aHL(6);
					break;
				case 0xF7:
					debug_print(byte, "SET 6, A");
					set_u3_r8(6, &cpu.regs[AF].high);
					break;
				case 0xF8:
					debug_print(byte, "SET 7, B");
					set_u3_r8(7, &cpu.regs[BC].high);
					break;
				case 0xF9:
					debug_print(byte, "SET 7, C");
					set_u3_r8(7, &cpu.regs[BC].low);
					break;
				case 0xFA:
					debug_print(byte, "SET 7, D");
					set_u3_r8(7, &cpu.regs[DE].high);
					break;
				case 0xFB:
					debug_print(byte, "SET 7, E");
					set_u3_r8(7, &cpu.regs[DE].low);
					break;
				case 0xFC:
					debug_print(byte, "SET 7, H");
					set_u3_r8(7, &cpu.regs[HL].high);
					break;
				case 0xFD:
					debug_print(byte, "SET 7, L");
					set_u3_r8(7, &cpu.regs[HL].low);
					break;
				case 0xFE:
					debug_print(byte, "SET 7, [HL]");
					set_u3_aHL(7);
					break;
				case 0xFF:
					debug_print(byte, "SET 7, A");
					set_u3_r8(7, &cpu.regs[AF].high);
					break;
				}
				cpu.prefix = false;
			} else {
				// ==================== NON-PREFIX ====================
				switch (byte) {
				case 0x00:
					debug_print(byte, "NOP");
					nop();
					break;
				case 0x01: {
					debug_print(byte, "LD BC, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ld_r16_n16(&cpu.regs[BC].full, n16);
				} break;
				case 0x02:
					debug_print(byte, "LD [BC], A");
					ld_a16_A(cpu.regs[BC].full);
					break;
				case 0x03:
					debug_print(byte, "INC BC");
					inc_r16(&cpu.regs[BC].full);
					break;
				case 0x04:
					debug_print(byte, "INC B");
					inc_r8(&cpu.regs[BC].high);
					break;
				case 0x05:
					debug_print(byte, "DEC B");
					dec_r8(&cpu.regs[BC].high);
					break;
				case 0x06: {
					debug_print(byte, "LD B, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_r8_n8(&cpu.regs[BC].high, n8);
				} break;
				case 0x07:
					debug_print(byte, "RLCA");
					rlca();
					break;
				case 0x08: {
					debug_print(byte, "LD [n16], SP");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ld_addr16_SP(n16);
				} break;
				case 0x09:
					debug_print(byte, "ADD HL, BC");
					add_HL_r16(cpu.regs[BC].full);
					break;
				case 0x0A:
					debug_print(byte, "LD A, [BC]");
					ld_A_a16(cpu.regs[BC].full);
					break;
				case 0x0B:
					debug_print(byte, "DEC BC");
					dec_r16(&cpu.regs[BC].full);
					break;
				case 0x0C:
					debug_print(byte, "INC C");
					inc_r8(&cpu.regs[BC].low);
					break;
				case 0x0D:
					debug_print(byte, "DEC C");
					dec_r8(&cpu.regs[BC].low);
					break;
				case 0x0E: {
					debug_print(byte, "LD C, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_r8_n8(&cpu.regs[BC].low, n8);
				} break;
				case 0x0F:
					debug_print(byte, "RRCA");
					rrca();
					break;
				case 0x10: {
					debug_print(byte, "STOP n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					stop_n8(n8);
				} break;
				case 0x11: {
					debug_print(byte, "LD DE, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ld_r16_n16(&cpu.regs[DE].full, n16);
				} break;
				case 0x12:
					debug_print(byte, "LD [DE], A");
					ld_a16_A(cpu.regs[DE].full);
					break;
				case 0x13:
					debug_print(byte, "INC DE");
					inc_r16(&cpu.regs[DE].full);
					break;
				case 0x14:
					debug_print(byte, "INC D");
					inc_r8(&cpu.regs[DE].high);
					break;
				case 0x15:
					debug_print(byte, "DEC D");
					dec_r8(&cpu.regs[DE].high);
					break;
				case 0x16: {
					debug_print(byte, "LD D, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_r8_n8(&cpu.regs[DE].high, n8);
				} break;
				case 0x17:
					debug_print(byte, "RLA");
					rla();
					break;
				case 0x18: {
					debug_print(byte, "JR n16");
					int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
					jr_n16(offset);
					break;
				}
				case 0x19:
					debug_print(byte, "ADD HL, DE");
					add_HL_r16(cpu.regs[DE].full);
					break;
				case 0x1A:
					debug_print(byte, "LD A, [DE]");
					ld_A_a16(cpu.regs[DE].full);
					break;
				case 0x1B:
					debug_print(byte, "DEC DE");
					dec_r16(&cpu.regs[DE].full);
					break;
				case 0x1C:
					debug_print(byte, "INC E");
					inc_r8(&cpu.regs[DE].low);
					break;
				case 0x1D:
					debug_print(byte, "DEC E");
					dec_r8(&cpu.regs[DE].low);
					break;
				case 0x1E: {
					debug_print(byte, "LD E, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_r8_n8(&cpu.regs[DE].low, n8);
				} break;
				case 0x1F:
					debug_print(byte, "RRA");
					rra();
					break;
				case 0x20: {
					debug_print(byte, "JR NZ, n16");
					int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
					jr_cc_n16(Z, false, offset);
					break;
				}
				case 0x21: {
					debug_print(byte, "LD HL, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ld_r16_n16(&cpu.regs[HL].full, n16);
				} break;
				case 0x22:
					debug_print(byte, "LD [HL+], A");
					ld_aHLi_A();
					break;
				case 0x23:
					debug_print(byte, "INC HL");
					inc_r16(&cpu.regs[HL].full);
					break;
				case 0x24:
					debug_print(byte, "INC H");
					inc_r8(&cpu.regs[HL].high);
					break;
				case 0x25:
					debug_print(byte, "DEC H");
					dec_r8(&cpu.regs[HL].high);
					break;
				case 0x26: {
					debug_print(byte, "LD H, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_r8_n8(&cpu.regs[HL].high, n8);
				} break;
				case 0x27:
					debug_print(byte, "DAA");
					daa();
					break;
				case 0x28: {
					debug_print(byte, "JR Z, n16");
					int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
					jr_cc_n16(Z, true, offset);
					break;
				}
				case 0x29:
					debug_print(byte, "ADD HL, HL");
					add_HL_r16(cpu.regs[HL].full);
					break;
				case 0x2A:
					debug_print(byte, "LD A, [HL+]");
					ld_A_aHLi();
					break;
				case 0x2B:
					debug_print(byte, "DEC HL");
					dec_r16(&cpu.regs[HL].full);
					break;
				case 0x2C:
					debug_print(byte, "INC L");
					inc_r8(&cpu.regs[HL].low);
					break;
				case 0x2D:
					debug_print(byte, "DEC L");
					dec_r8(&cpu.regs[HL].low);
					break;
				case 0x2E: {
					debug_print(byte, "LD L, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_r8_n8(&cpu.regs[HL].low, n8);
				} break;
				case 0x2F:
					debug_print(byte, "CPL");
					cpl();
					break;
				case 0x30: {
					debug_print(byte, "JR NC, n16");
					int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
					jr_cc_n16(C, false, offset);
					break;
				}
				case 0x31: {
					debug_print(byte, "LD SP, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ld_r16_n16(&cpu.regs[SP].full, n16);
				} break;
				case 0x32:
					debug_print(byte, "LD [HL-], A");
					ld_aHLd_A();
					break;
				case 0x33:
					debug_print(byte, "INC SP");
					inc_r16(&cpu.regs[SP].full);
					break;
				case 0x34:
					debug_print(byte, "INC [HL]");
					inc_aHL();
					break;
				case 0x35:
					debug_print(byte, "DEC [HL]");
					dec_aHL();
					break;
				case 0x36: {
					debug_print(byte, "LD [HL], n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_aHL_n8(n8);
				} break;
				case 0x37:
					debug_print(byte, "SCF");
					scf();
					break;
				case 0x38: {
					debug_print(byte, "JR C, n16");
					int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
					jr_cc_n16(C, true, offset);
					break;
				}
				case 0x39:
					debug_print(byte, "ADD HL, SP");
					add_HL_r16(cpu.regs[SP].full);
					break;
				case 0x3A:
					debug_print(byte, "LD A, [HL-]");
					ld_A_aHLd();
					break;
				case 0x3B:
					debug_print(byte, "DEC SP");
					dec_r16(&cpu.regs[SP].full);
					break;
				case 0x3C:
					debug_print(byte, "INC A");
					inc_r8(&cpu.regs[AF].high);
					break;
				case 0x3D:
					debug_print(byte, "DEC A");
					dec_r8(&cpu.regs[AF].high);
					break;
				case 0x3E: {
					debug_print(byte, "LD A, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					ld_r8_n8(&cpu.regs[AF].high, n8);
				} break;
				case 0x3F:
					debug_print(byte, "CCF");
					ccf();
					break;
				case 0x40:
					debug_print(byte, "LD B, B");
					ld_r8_r8(&cpu.regs[BC].high, cpu.regs[BC].high);
					break;
				case 0x41:
					debug_print(byte, "LD B, C");
					ld_r8_r8(&cpu.regs[BC].high, cpu.regs[BC].low);
					break;
				case 0x42:
					debug_print(byte, "LD B, D");
					ld_r8_r8(&cpu.regs[BC].high, cpu.regs[DE].high);
					break;
				case 0x43:
					debug_print(byte, "LD B, E");
					ld_r8_r8(&cpu.regs[BC].high, cpu.regs[DE].low);
					break;
				case 0x44:
					debug_print(byte, "LD B, H");
					ld_r8_r8(&cpu.regs[BC].high, cpu.regs[HL].high);
					break;
				case 0x45:
					debug_print(byte, "LD B, L");
					ld_r8_r8(&cpu.regs[BC].high, cpu.regs[HL].low);
					break;
				case 0x46:
					debug_print(byte, "LD B, [HL]");
					ld_r8_aHL(&cpu.regs[BC].high);
					break;
				case 0x47:
					debug_print(byte, "LD B, A");
					ld_r8_r8(&cpu.regs[BC].high, cpu.regs[AF].high);
					break;
				case 0x48:
					debug_print(byte, "LD C, B");
					ld_r8_r8(&cpu.regs[BC].low, cpu.regs[BC].high);
					break;
				case 0x49:
					debug_print(byte, "LD C, C");
					ld_r8_r8(&cpu.regs[BC].low, cpu.regs[BC].low);
					break;
				case 0x4A:
					debug_print(byte, "LD C, D");
					ld_r8_r8(&cpu.regs[BC].low, cpu.regs[DE].high);
					break;
				case 0x4B:
					debug_print(byte, "LD C, E");
					ld_r8_r8(&cpu.regs[BC].low, cpu.regs[DE].low);
					break;
				case 0x4C:
					debug_print(byte, "LD C, H");
					ld_r8_r8(&cpu.regs[BC].low, cpu.regs[HL].high);
					break;
				case 0x4D:
					debug_print(byte, "LD C, L");
					ld_r8_r8(&cpu.regs[BC].low, cpu.regs[HL].low);
					break;
				case 0x4E:
					debug_print(byte, "LD C, [HL]");
					ld_r8_aHL(&cpu.regs[BC].low);
					break;
				case 0x4F:
					debug_print(byte, "LD C, A");
					ld_r8_r8(&cpu.regs[BC].low, cpu.regs[AF].high);
					break;
				case 0x50:
					debug_print(byte, "LD D, B");
					ld_r8_r8(&cpu.regs[DE].high, cpu.regs[BC].high);
					break;
				case 0x51:
					debug_print(byte, "LD D, C");
					ld_r8_r8(&cpu.regs[DE].high, cpu.regs[BC].low);
					break;
				case 0x52:
					debug_print(byte, "LD D, D");
					ld_r8_r8(&cpu.regs[DE].high, cpu.regs[DE].high);
					break;
				case 0x53:
					debug_print(byte, "LD D, E");
					ld_r8_r8(&cpu.regs[DE].high, cpu.regs[DE].low);
					break;
				case 0x54:
					debug_print(byte, "LD D, H");
					ld_r8_r8(&cpu.regs[DE].high, cpu.regs[HL].high);
					break;
				case 0x55:
					debug_print(byte, "LD D, L");
					ld_r8_r8(&cpu.regs[DE].high, cpu.regs[HL].low);
					break;
				case 0x56:
					debug_print(byte, "LD D, [HL]");
					ld_r8_aHL(&cpu.regs[DE].high);
					break;
				case 0x57:
					debug_print(byte, "LD D, A");
					ld_r8_r8(&cpu.regs[DE].high, cpu.regs[AF].high);
					break;
				case 0x58:
					debug_print(byte, "LD E, B");
					ld_r8_r8(&cpu.regs[DE].low, cpu.regs[BC].high);
					break;
				case 0x59:
					debug_print(byte, "LD E, C");
					ld_r8_r8(&cpu.regs[DE].low, cpu.regs[BC].low);
					break;
				case 0x5A:
					debug_print(byte, "LD E, D");
					ld_r8_r8(&cpu.regs[DE].low, cpu.regs[DE].high);
					break;
				case 0x5B:
					debug_print(byte, "LD E, E");
					ld_r8_r8(&cpu.regs[DE].low, cpu.regs[DE].low);
					break;
				case 0x5C:
					debug_print(byte, "LD E, H");
					ld_r8_r8(&cpu.regs[DE].low, cpu.regs[HL].high);
					break;
				case 0x5D:
					debug_print(byte, "LD E, L");
					ld_r8_r8(&cpu.regs[DE].low, cpu.regs[HL].low);
					break;
				case 0x5E:
					debug_print(byte, "LD E, [HL]");
					ld_r8_aHL(&cpu.regs[DE].low);
					break;
				case 0x5F:
					debug_print(byte, "LD E, A");
					ld_r8_r8(&cpu.regs[DE].low, cpu.regs[AF].high);
					break;
				case 0x60:
					debug_print(byte, "LD H, B");
					ld_r8_r8(&cpu.regs[HL].high, cpu.regs[BC].high);
					break;
				case 0x61:
					debug_print(byte, "LD H, C");
					ld_r8_r8(&cpu.regs[HL].high, cpu.regs[BC].low);
					break;
				case 0x62:
					debug_print(byte, "LD H, D");
					ld_r8_r8(&cpu.regs[HL].high, cpu.regs[DE].high);
					break;
				case 0x63:
					debug_print(byte, "LD H, E");
					ld_r8_r8(&cpu.regs[HL].high, cpu.regs[DE].low);
					break;
				case 0x64:
					debug_print(byte, "LD H, H");
					ld_r8_r8(&cpu.regs[HL].high, cpu.regs[HL].high);
					break;
				case 0x65:
					debug_print(byte, "LD H, L");
					ld_r8_r8(&cpu.regs[HL].high, cpu.regs[HL].low);
					break;
				case 0x66:
					debug_print(byte, "LD H, [HL]");
					ld_r8_aHL(&cpu.regs[HL].high);
					break;
				case 0x67:
					debug_print(byte, "LD H, A");
					ld_r8_r8(&cpu.regs[HL].high, cpu.regs[AF].high);
					break;
				case 0x68:
					debug_print(byte, "LD L, B");
					ld_r8_r8(&cpu.regs[HL].low, cpu.regs[BC].high);
					break;
				case 0x69:
					debug_print(byte, "LD L, C");
					ld_r8_r8(&cpu.regs[HL].low, cpu.regs[BC].low);
					break;
				case 0x6A:
					debug_print(byte, "LD L, D");
					ld_r8_r8(&cpu.regs[HL].low, cpu.regs[DE].high);
					break;
				case 0x6B:
					debug_print(byte, "LD L, E");
					ld_r8_r8(&cpu.regs[HL].low, cpu.regs[DE].low);
					break;
				case 0x6C:
					debug_print(byte, "LD L, H");
					ld_r8_r8(&cpu.regs[HL].low, cpu.regs[HL].high);
					break;
				case 0x6D:
					debug_print(byte, "LD L, L");
					ld_r8_r8(&cpu.regs[HL].low, cpu.regs[HL].low);
					break;
				case 0x6E:
					debug_print(byte, "LD L, [HL]");
					ld_r8_aHL(&cpu.regs[HL].low);
					break;
				case 0x6F:
					debug_print(byte, "LD L, A");
					ld_r8_r8(&cpu.regs[HL].low, cpu.regs[AF].high);
					break;
				case 0x70:
					debug_print(byte, "LD [HL], B");
					ld_aHL_r8(cpu.regs[BC].high);
					break;
				case 0x71:
					debug_print(byte, "LD [HL], C");
					ld_aHL_r8(cpu.regs[BC].low);
					break;
				case 0x72:
					debug_print(byte, "LD [HL], D");
					ld_aHL_r8(cpu.regs[DE].high);
					break;
				case 0x73:
					debug_print(byte, "LD [HL], E");
					ld_aHL_r8(cpu.regs[DE].low);
					break;
				case 0x74:
					debug_print(byte, "LD [HL], H");
					ld_aHL_r8(cpu.regs[HL].high);
					break;
				case 0x75:
					debug_print(byte, "LD [HL], L");
					ld_aHL_r8(cpu.regs[HL].low);
					break;
				case 0x76:
					debug_print(byte, "HALT");
					halt();
					break;
				case 0x77:
					debug_print(byte, "LD [HL], A");
					ld_aHL_r8(cpu.regs[AF].high);
					break;
				case 0x78:
					debug_print(byte, "LD A, B");
					ld_r8_r8(&cpu.regs[AF].high, cpu.regs[BC].high);
					break;
				case 0x79:
					debug_print(byte, "LD A, C");
					ld_r8_r8(&cpu.regs[AF].high, cpu.regs[BC].low);
					break;
				case 0x7A:
					debug_print(byte, "LD A, D");
					ld_r8_r8(&cpu.regs[AF].high, cpu.regs[DE].high);
					break;
				case 0x7B:
					debug_print(byte, "LD A, E");
					ld_r8_r8(&cpu.regs[AF].high, cpu.regs[DE].low);
					break;
				case 0x7C:
					debug_print(byte, "LD A, H");
					ld_r8_r8(&cpu.regs[AF].high, cpu.regs[HL].high);
					break;
				case 0x7D:
					debug_print(byte, "LD A, L");
					ld_r8_r8(&cpu.regs[AF].high, cpu.regs[HL].low);
					break;
				case 0x7E:
					debug_print(byte, "LD A, [HL]");
					ld_r8_aHL(&cpu.regs[AF].high);
					break;
				case 0x7F:
					debug_print(byte, "LD A, A");
					ld_r8_r8(&cpu.regs[AF].high, cpu.regs[AF].high);
					break;
				case 0x80:
					debug_print(byte, "ADD A, B");
					add_A_r8(cpu.regs[BC].high);
					break;
				case 0x81:
					debug_print(byte, "ADD A, C");
					add_A_r8(cpu.regs[BC].low);
					break;
				case 0x82:
					debug_print(byte, "ADD A, D");
					add_A_r8(cpu.regs[DE].high);
					break;
				case 0x83:
					debug_print(byte, "ADD A, E");
					add_A_r8(cpu.regs[DE].low);
					break;
				case 0x84:
					debug_print(byte, "ADD A, H");
					add_A_r8(cpu.regs[HL].high);
					break;
				case 0x85:
					debug_print(byte, "ADD A, L");
					add_A_r8(cpu.regs[HL].low);
					break;
				case 0x86:
					debug_print(byte, "ADD A, [HL]");
					add_A_aHL();
					break;
				case 0x87:
					debug_print(byte, "ADD A, A");
					add_A_r8(cpu.regs[AF].high);
					break;
				case 0x88:
					debug_print(byte, "ADC A, B");
					adc_A_r8(cpu.regs[BC].high);
					break;
				case 0x89:
					debug_print(byte, "ADC A, C");
					adc_A_r8(cpu.regs[BC].low);
					break;
				case 0x8A:
					debug_print(byte, "ADC A, D");
					adc_A_r8(cpu.regs[DE].high);
					break;
				case 0x8B:
					debug_print(byte, "ADC A, E");
					adc_A_r8(cpu.regs[DE].low);
					break;
				case 0x8C:
					debug_print(byte, "ADC A, H");
					adc_A_r8(cpu.regs[HL].high);
					break;
				case 0x8D:
					debug_print(byte, "ADC A, L");
					adc_A_r8(cpu.regs[HL].low);
					break;
				case 0x8E:
					debug_print(byte, "ADC A, [HL]");
					adc_A_aHL();
					break;
				case 0x8F:
					debug_print(byte, "ADC A, A");
					adc_A_r8(cpu.regs[AF].high);
					break;
				case 0x90:
					debug_print(byte, "SUB A, B");
					sub_A_r8(cpu.regs[BC].high);
					break;
				case 0x91:
					debug_print(byte, "SUB A, C");
					sub_A_r8(cpu.regs[BC].low);
					break;
				case 0x92:
					debug_print(byte, "SUB A, D");
					sub_A_r8(cpu.regs[DE].high);
					break;
				case 0x93:
					debug_print(byte, "SUB A, E");
					sub_A_r8(cpu.regs[DE].low);
					break;
				case 0x94:
					debug_print(byte, "SUB A, H");
					sub_A_r8(cpu.regs[HL].high);
					break;
				case 0x95:
					debug_print(byte, "SUB A, L");
					sub_A_r8(cpu.regs[HL].low);
					break;
				case 0x96:
					debug_print(byte, "SUB A, [HL]");
					sub_A_aHL();
					break;
				case 0x97:
					debug_print(byte, "SUB A, A");
					sub_A_r8(cpu.regs[AF].high);
					break;
				case 0x98:
					debug_print(byte, "SBC A, B");
					sbc_A_r8(cpu.regs[BC].high);
					break;
				case 0x99:
					debug_print(byte, "SBC A, C");
					sbc_A_r8(cpu.regs[BC].low);
					break;
				case 0x9A:
					debug_print(byte, "SBC A, D");
					sbc_A_r8(cpu.regs[DE].high);
					break;
				case 0x9B:
					debug_print(byte, "SBC A, E");
					sbc_A_r8(cpu.regs[DE].low);
					break;
				case 0x9C:
					debug_print(byte, "SBC A, H");
					sbc_A_r8(cpu.regs[HL].high);
					break;
				case 0x9D:
					debug_print(byte, "SBC A, L");
					sbc_A_r8(cpu.regs[HL].low);
					break;
				case 0x9E:
					debug_print(byte, "SBC A, [HL]");
					sbc_A_aHL();
					break;
				case 0x9F:
					debug_print(byte, "SBC A, A");
					sbc_A_r8(cpu.regs[AF].high);
					break;
				case 0xA0:
					debug_print(byte, "AND A, B");
					and_A_r8(cpu.regs[BC].high);
					break;
				case 0xA1:
					debug_print(byte, "AND A, C");
					and_A_r8(cpu.regs[BC].low);
					break;
				case 0xA2:
					debug_print(byte, "AND A, D");
					and_A_r8(cpu.regs[DE].high);
					break;
				case 0xA3:
					debug_print(byte, "AND A, E");
					and_A_r8(cpu.regs[DE].low);
					break;
				case 0xA4:
					debug_print(byte, "AND A, H");
					and_A_r8(cpu.regs[HL].high);
					break;
				case 0xA5:
					debug_print(byte, "AND A, L");
					and_A_r8(cpu.regs[HL].low);
					break;
				case 0xA6:
					debug_print(byte, "AND A, [HL]");
					and_A_aHL();
					break;
				case 0xA7:
					debug_print(byte, "AND A, A");
					and_A_r8(cpu.regs[AF].high);
					break;
				case 0xA8:
					debug_print(byte, "XOR A, B");
					xor_A_r8(cpu.regs[BC].high);
					break;
				case 0xA9:
					debug_print(byte, "XOR A, C");
					xor_A_r8(cpu.regs[BC].low);
					break;
				case 0xAA:
					debug_print(byte, "XOR A, D");
					xor_A_r8(cpu.regs[DE].high);
					break;
				case 0xAB:
					debug_print(byte, "XOR A, E");
					xor_A_r8(cpu.regs[DE].low);
					break;
				case 0xAC:
					debug_print(byte, "XOR A, H");
					xor_A_r8(cpu.regs[HL].high);
					break;
				case 0xAD:
					debug_print(byte, "XOR A, L");
					xor_A_r8(cpu.regs[HL].low);
					break;
				case 0xAE:
					debug_print(byte, "XOR A, [HL]");
					xor_A_aHL();
					break;
				case 0xAF:
					debug_print(byte, "XOR A, A");
					xor_A_r8(cpu.regs[AF].high);
					break;
				case 0xB0:
					debug_print(byte, "OR A, B");
					or_A_r8(cpu.regs[BC].high);
					break;
				case 0xB1:
					debug_print(byte, "OR A, C");
					or_A_r8(cpu.regs[BC].low);
					break;
				case 0xB2:
					debug_print(byte, "OR A, D");
					or_A_r8(cpu.regs[DE].high);
					break;
				case 0xB3:
					debug_print(byte, "OR A, E");
					or_A_r8(cpu.regs[DE].low);
					break;
				case 0xB4:
					debug_print(byte, "OR A, H");
					or_A_r8(cpu.regs[HL].high);
					break;
				case 0xB5:
					debug_print(byte, "OR A, L");
					or_A_r8(cpu.regs[HL].low);
					break;
				case 0xB6:
					debug_print(byte, "OR A, [HL]");
					or_A_aHL();
					break;
				case 0xB7:
					debug_print(byte, "OR A, A");
					or_A_r8(cpu.regs[AF].high);
					break;
				case 0xB8:
					debug_print(byte, "CP A, B");
					cp_A_r8(cpu.regs[BC].high);
					break;
				case 0xB9:
					debug_print(byte, "CP A, C");
					cp_A_r8(cpu.regs[BC].low);
					break;
				case 0xBA:
					debug_print(byte, "CP A, D");
					cp_A_r8(cpu.regs[DE].high);
					break;
				case 0xBB:
					debug_print(byte, "CP A, E");
					cp_A_r8(cpu.regs[DE].low);
					break;
				case 0xBC:
					debug_print(byte, "CP A, H");
					cp_A_r8(cpu.regs[HL].high);
					break;
				case 0xBD:
					debug_print(byte, "CP A, L");
					cp_A_r8(cpu.regs[HL].low);
					break;
				case 0xBE:
					debug_print(byte, "CP A, [HL]");
					cp_A_aHL();
					break;
				case 0xBF:
					debug_print(byte, "CP A, A");
					cp_A_r8(cpu.regs[AF].high);
					break;
				case 0xC0:
					debug_print(byte, "RET NZ");
					ret_cc(Z, false);
					break;
				case 0xC1:
					debug_print(byte, "POP BC");
					pop_r16(&cpu.regs[BC].full);
					break;
				case 0xC2: {
					debug_print(byte, "JP NZ, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					jp_cc_n16(Z, false, n16);
					break;
				}
				case 0xC3: {
					debug_print(byte, "JP n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					jp_n16(n16);
					break;
				}
				case 0xC4: {
					debug_print(byte, "CALL NZ, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					call_cc_n16(Z, false, n16);
					break;
				}
				case 0xC5:
					debug_print(byte, "PUSH BC");
					push_r16(cpu.regs[BC].full);
					break;
				case 0xC6: {
					debug_print(byte, "ADD A, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					add_A_n8(n8);
					break;
				}
				case 0xC7:
					debug_print(byte, "RST $00");
					rst(0x00);
					break;
				case 0xC8:
					debug_print(byte, "RET Z");
					ret_cc(Z, true);
					break;
				case 0xC9:
					debug_print(byte, "RET");
					ret();
					break;
				case 0xCA: {
					debug_print(byte, "JP Z, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					jp_cc_n16(Z, true, n16);
					break;
				}
				case 0xCB:
					debug_print(byte, "PREFIX TOGGLE");
					cpu.prefix = true;
					cpu.regs[PC].full += 1;
					break;
				case 0xCC: {
					debug_print(byte, "CALL Z, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					call_cc_n16(Z, true, n16);
					break;
				}
				case 0xCD: {
					debug_print(byte, "CALL n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					call_n16(n16);
					break;
				}
				case 0xCE: {
					debug_print(byte, "ADC A, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					adc_A_n8(n8);
					break;
				}
				case 0xCF:
					debug_print(byte, "RST $08");
					rst(0x08);
					break;
				case 0xD0:
					debug_print(byte, "RET NC");
					ret_cc(C, false);
					break;
				case 0xD1:
					debug_print(byte, "POP DE");
					pop_r16(&cpu.regs[DE].full);
					break;
				case 0xD2: {
					debug_print(byte, "JP NC, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					jp_cc_n16(C, false, n16);
					break;
				}
				case 0xD4: {
					debug_print(byte, "CALL NC, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					call_cc_n16(C, false, n16);
					break;
				}
				case 0xD5:
					debug_print(byte, "PUSH DE");
					push_r16(cpu.regs[DE].full);
					break;
				case 0xD6: {
					debug_print(byte, "SUB A, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					sub_A_n8(n8);
					break;
				}
				case 0xD7:
					debug_print(byte, "RST $10");
					rst(0x10);
					break;
				case 0xD8:
					debug_print(byte, "RET C");
					ret_cc(C, true);
					break;
				case 0xD9:
					debug_print(byte, "RETI");
					reti();
					break;
				case 0xDA: {
					debug_print(byte, "JP C, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					jp_cc_n16(C, true, n16);
					break;
				}
				case 0xDC: {
					debug_print(byte, "CALL C, n16");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					call_cc_n16(C, true, n16);
					break;
				}
				case 0xDE: {
					debug_print(byte, "SBC A, n8");
					uint8_t n8 = read8(cpu.regs[PC].full + 1);
					sbc_A_n8(n8);
					break;
				}
				case 0xDF:
					debug_print(byte, "RST $18");
					rst(0x18);
					break;
				case 0xE0: {
					debug_print(byte, "LDH [n16], A");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ldh_addr16_A(n16);
					break;
				}
				case 0xE1:
					debug_print(byte, "POP HL");
					pop_r16(&cpu.regs[HL].full);
					break;
				case 0xE2:
					debug_print(byte, "LDH [C], A");
					ldh_aC_A();
					break;
				case 0xE5:
					debug_print(byte, "PUSH HL");
					push_r16(cpu.regs[HL].full);
					break;
				case 0xE6: {
					debug_print(byte, "AND A, n8");
					int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
					and_A_n8(n8);
				} break;
				case 0xE7:
					debug_print(byte, "RST $20");
					rst(0x20);
					break;
				case 0xE8: {
					debug_print(byte, "ADD SP, n8");
					int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
					add_SP_n8(n8);
				} break;
				case 0xE9:
					debug_print(byte, "JP HL");
					jp_aHL();
					break;
				case 0xEA: {
					debug_print(byte, "LD [n16], A");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ld_addr16_A(n16);
					break;
				}
				case 0xEE: {
					debug_print(byte, "XOR A, n8");
					int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
					xor_A_n8(n8);
				} break;
				case 0xEF:
					debug_print(byte, "RST $28");
					rst(0x28);
					break;
				case 0xF0: {
					debug_print(byte, "LDH A, [n16]");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ldh_A_addr16(n16);
					break;
				}
				case 0xF1:
					debug_print(byte, "POP AF");
					pop_r16(&cpu.regs[AF].full);
					break;
				case 0xF2:
					debug_print(byte, "LDH A, [C]");
					ldh_A_aC();
					break;
				case 0xF3:
					debug_print(byte, "DI");
					di();
					break;
				case 0xF5:
					debug_print(byte, "PUSH AF");
					push_r16(cpu.regs[AF].full);
					break;
				case 0xF6: {
					debug_print(byte, "OR A, n8");
					int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
					or_A_n8(n8);
				} break;
				case 0xF7:
					debug_print(byte, "RST $30");
					rst(0x30);
					break;
				case 0xF8: {
					debug_print(byte, "LD HL, SP+n8");
					int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
					ld_HL_SPe8(n8);
				} break;
				case 0xF9:
					debug_print(byte, "LD SP, HL");
					ld_SP_HL();
					break;
				case 0xFA: {
					debug_print(byte, "LD A, [n16]");
					uint16_t n16 = read16(cpu.regs[PC].full + 1);
					ld_A_addr16(n16);
					break;
				}
				case 0xFB:
					debug_print(byte, "EI");
					ei();
					break;
				case 0xFE: {
					debug_print(byte, "CP A, n8");
					int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
					cp_A_n8(n8);
				} break;
				case 0xFF:
					debug_print(byte, "RST $38");
					rst(0x38);
					break;
				default:
					debug_print(byte, "INVALID OPCODE");
					cpu.regs[PC].full += 1;
					cpu.cycle += 1;
					break;
				}
			}

			switch (cpu.ime_enable_counter) {
			case 1:
				--cpu.ime_enable_counter;
				break;
			case 0:
				--cpu.ime_enable_counter;
				cpu.ime = true;
				break;
			default:
				break;
			}

			timer.divider_register_cycle_counter += cpu.cycle - cycle_checkpoint;
			if (timer.divider_register_cycle_counter >= CYCLES_PER_DIV) {
				++timer.divider_register;
				timer.divider_register_cycle_counter = 0;
			}

			timer.timer_counter_cycle_counter += cpu.cycle - cycle_checkpoint;
			if (timer.timer_counter_cycle_counter >= CPU_FREQ / timer.timer_control) {
				if (timer.timer_counter == UINT8_MAX)
					timer.timer_counter = timer.timer_modulo;
				else
					++timer.timer_counter;
				timer.timer_counter_cycle_counter = 0;
			}
		}

		const uint64_t execution_time = SDL_GetTicks() - frame_start_time;
		if (execution_time < MS_PER_FRAME)
			SDL_Delay(MS_PER_FRAME - execution_time);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
