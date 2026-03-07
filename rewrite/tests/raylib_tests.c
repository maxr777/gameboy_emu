bool compareCartridgeHeader(CartridgeHeader *input, CartridgeHeader *output) {
	for (int i = 0; i < 4; ++i) {
		if (input->entry_point[i] != output->entry_point[i])
			return false;
	}

	for (int i = 0; i < 48; ++i) {
		if (input->nintendo_logo[i] != output->nintendo_logo[i])
			return false;
	}

	for (int i = 0; i < 16; ++i) {
		if (input->title[i] != output->title[i])
			return false;
	}

	if (input->cgb_flag != output->cgb_flag)
		return false;

	if (input->new_license_code != output->new_license_code)
		return false;

	if (input->sgb_flag != output->sgb_flag)
		return false;

	if (input->cartridge_type != output->cartridge_type)
		return false;

	if (input->rom_size != output->rom_size)
		return false;

	if (input->ram_size != output->ram_size)
		return false;

	if (input->destination_code != output->destination_code)
		return false;

	if (input->old_licensee_code != output->old_licensee_code)
		return false;

	if (input->mask_rom_version_number != output->mask_rom_version_number)
		return false;

	if (input->header_checksum != output->header_checksum)
		return false;

	if (input->global_checksum != output->global_checksum)
		return false;

	return true;
}

void checkLoadGame(const char *filepath, ROM *expected) {
	ROM rom = {};
	assert(loadGame(filepath, &rom) == 0);

	assert(rom.game_size == expected->game_size);
	assert(compareCartridgeHeader(&rom.cartridge_header, &expected->cartridge_header));
	assert(rom.boot_rom_enabled == expected->boot_rom_enabled);

	for (int i = 0; i < 256; ++i) {
		assert(rom.boot_rom[i] == expected->boot_rom[i]);
	}

	assert(rom.max_rom_banks == expected->max_rom_banks);
}

void testLoadGame() {
	ROM rom = {};
}

void runTests() {
	testLoadGame();
}
