// bool compareCartridgeHeader(const CartridgeHeader *input, const CartridgeHeader *output) {
// 	for (int i = 0; i < 4; ++i) {
// 		if (input->entry_point[i] != output->entry_point[i])
// 			return false;
// 	}
//
// 	for (int i = 0; i < 48; ++i) {
// 		if (input->nintendo_logo[i] != output->nintendo_logo[i])
// 			return false;
// 	}
//
// 	for (int i = 0; i < 16; ++i) {
// 		if (input->title[i] != output->title[i])
// 			return false;
// 	}
//
// 	if (input->cgb_flag != output->cgb_flag)
// 		return false;
//
// 	if (input->new_license_code != output->new_license_code)
// 		return false;
//
// 	if (input->sgb_flag != output->sgb_flag)
// 		return false;
//
// 	if (input->cartridge_type != output->cartridge_type)
// 		return false;
//
// 	if (input->rom_size != output->rom_size)
// 		return false;
//
// 	if (input->ram_size != output->ram_size)
// 		return false;
//
// 	if (input->destination_code != output->destination_code)
// 		return false;
//
// 	if (input->old_licensee_code != output->old_licensee_code)
// 		return false;
//
// 	if (input->mask_rom_version_number != output->mask_rom_version_number)
// 		return false;
//
// 	if (input->header_checksum != output->header_checksum)
// 		return false;
//
// 	if (input->global_checksum != output->global_checksum)
// 		return false;
//
// 	return true;
// }

/**
 * @brief Tests platformLoadGame() - see extended description
 *
 * @attention It only tests some of the fields - for example, I don't test if the actual game has been correctly loaded
 * (I assume fread() works correctly). Similarly, since I just hardcode the values for the expected ROM,
 * I only fill in certain fields (look at testLoadGame()). It should be good enough, but it's not 100% thorough.
 *
 */
void checkLoadGame(const char *filepath, const ROM *expected) {
	ROM rom = {};

	{
		int ret_code = platformLoadGame(filepath, &rom);
		if (ret_code != 0) {
			fprintf(stderr, "platformLoadGame error: %d\n", ret_code);
			return;
		}
	}

	assert(rom.game_size == expected->game_size);

	// I only tests select few cartridge_header fields - all the ones set by rgbfix in tests/compile_roms.sh
	assert(!strcmp(rom.cartridge_header.title, expected->cartridge_header.title));
	assert(rom.cartridge_header.cgb_flag == expected->cartridge_header.cgb_flag);
	assert(rom.cartridge_header.new_license_code[0] == expected->cartridge_header.new_license_code[0]);
	assert(rom.cartridge_header.new_license_code[1] == expected->cartridge_header.new_license_code[1]);
	assert(rom.cartridge_header.cartridge_type == expected->cartridge_header.cartridge_type);
	assert(rom.cartridge_header.ram_size == expected->cartridge_header.ram_size);
	assert(rom.cartridge_header.mask_rom_version_number == expected->cartridge_header.mask_rom_version_number);

	assert(rom.boot_rom_enabled == expected->boot_rom_enabled);
	assert(rom.boot_rom == expected->boot_rom);
}

/**
 * @brief Wrapper for testPlatformLoadGame()
 */
void testLoadGame() {
	ROM expected = {};

	expected.game_size = 16384;
	strncpy(expected.cartridge_header.title, "HEADER_TEST", sizeof(expected.cartridge_header.title) - 1);
	expected.cartridge_header.title[15] = '\0';
	expected.cartridge_header.cgb_flag = 0;
	expected.cartridge_header.new_license_code[0] = '4';
	expected.cartridge_header.new_license_code[1] = 'Z';
	expected.cartridge_header.cartridge_type = 0x01;
	expected.cartridge_header.ram_size = 0x00;
	expected.cartridge_header.mask_rom_version_number = 0x03;

	expected.boot_rom = BOOT_ROM;

	// at first I used "../tests/cartridge_header_test.gb" because this line gets ran in platform/raylib_gameboy.c
	// but fopen() uses CWD - since I run it with ./run.sh, which is one layer above, it didn't work
	// that's why it's "tests/" and not "../tests/"
	checkLoadGame("tests/cartridge_header_test.gb", &expected);
}

void runTests() {
	testLoadGame();
}
