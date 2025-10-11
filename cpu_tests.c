#include "gameboy.h"
#define TEST_NUM 1000

typedef struct {
	uint8_t byte1;
	// if opcode uses only 1 byte, the rest is irrelevant
	uint8_t byte2;
	uint8_t byte3;
} Instruction;

typedef struct {
	Register registers[REGISTER_COUNT];
	int current_cycle;
	bool is_current_opcode_prefixed;
} CPU_State;

int main() {
	Instruction input_array[TEST_NUM] = {0};
	CPU_State input_state[TEST_NUM] = {0};
	CPU_State output_state[TEST_NUM] = {0};

	// ==================== NOP ====================

	input_array[0].byte1 = 0x00;
	input_array[0].byte2 = 0x00;
	input_array[0].byte3 = 0x00;

	input_state[0].registers[AF].full = 0x1234;
	input_state[0].registers[BC].full = 0x1110;
	input_state[0].registers[DE].full = 0x9300;
	input_state[0].registers[HL].full = 0x0834;
	input_state[0].registers[SP].full = 0x0003;
	input_state[0].registers[PC].full = 0x0107;
	input_state[0].current_cycle = 7;
	input_state[0].is_current_opcode_prefixed = 0;

	output_state[0].registers[AF].full = 0x1234;
	output_state[0].registers[BC].full = 0x1110;
	output_state[0].registers[DE].full = 0x9300;
	output_state[0].registers[HL].full = 0x0834;
	output_state[0].registers[SP].full = 0x0003;
	output_state[0].registers[PC].full = 0x0107;
	output_state[0].current_cycle = 8;
	output_state[0].is_current_opcode_prefixed = 0;

	// ==================== LD r16 n16 ====================

	input_array[0].byte1 = 0x01;
	input_array[0].byte2 = 0x12;
	input_array[0].byte3 = 0x98;

	input_state[0].registers[AF].full = 0x1200;
	input_state[0].registers[BC].full = 0x1110;
	input_state[0].registers[DE].full = 0x9300;
	input_state[0].registers[HL].full = 0x0834;
	input_state[0].registers[SP].full = 0x0003;
	input_state[0].registers[PC].full = 0x0107;
	input_state[0].current_cycle = 7;
	input_state[0].is_current_opcode_prefixed = 0;

	output_state[0].registers[AF].full = 0x1200;
	output_state[0].registers[BC].full = 0x1298;
	output_state[0].registers[DE].full = 0x9300;
	output_state[0].registers[HL].full = 0x0834;
	output_state[0].registers[SP].full = 0x0003;
	output_state[0].registers[PC].full = 0x0107;
	output_state[0].current_cycle = 10;
	output_state[0].is_current_opcode_prefixed = 0;

	for (int i = 0; i < TEST_NUM; ++i) {
		if (input_state[i].is_current_opcode_prefixed) {
			// ==================== YES-PREFIX ====================
			switch (byte) {
			case 0x00:
				rlc_r8(&cpu.regs[BC].high);
				break;
			case 0x01:
				rlc_r8(&cpu.regs[BC].low);
				break;
			case 0x02:
				rlc_r8(&cpu.regs[DE].high);
				break;
			case 0x03:
				rlc_r8(&cpu.regs[DE].low);
				break;
			case 0x04:
				rlc_r8(&cpu.regs[HL].high);
				break;
			case 0x05:
				rlc_r8(&cpu.regs[HL].low);
				break;
			case 0x06:
				rlc_aHL();
				break;
			case 0x07:
				rlc_r8(&cpu.regs[AF].high);
				break;
			case 0x08:
				rrc_r8(&cpu.regs[BC].high);
				break;
			case 0x09:
				rrc_r8(&cpu.regs[BC].low);
				break;
			case 0x0A:
				rrc_r8(&cpu.regs[DE].high);
				break;
			case 0x0B:
				rrc_r8(&cpu.regs[DE].low);
				break;
			case 0x0C:
				rrc_r8(&cpu.regs[HL].high);
				break;
			case 0x0D:
				rrc_r8(&cpu.regs[HL].low);
				break;
			case 0x0E:
				rrc_aHL();
				break;
			case 0x0F:
				rrc_r8(&cpu.regs[AF].high);
				break;
			case 0x10:
				rl_r8(&cpu.regs[BC].high);
				break;
			case 0x11:
				rl_r8(&cpu.regs[BC].low);
				break;
			case 0x12:
				rl_r8(&cpu.regs[DE].high);
				break;
			case 0x13:
				rl_r8(&cpu.regs[DE].low);
				break;
			case 0x14:
				rl_r8(&cpu.regs[HL].high);
				break;
			case 0x15:
				rl_r8(&cpu.regs[HL].low);
				break;
			case 0x16:
				rl_aHL();
				break;
			case 0x17:
				rl_r8(&cpu.regs[AF].high);
				break;
			case 0x18:
				rr_r8(&cpu.regs[BC].high);
				break;
			case 0x19:
				rr_r8(&cpu.regs[BC].low);
				break;
			case 0x1A:
				rr_r8(&cpu.regs[DE].high);
				break;
			case 0x1B:
				rr_r8(&cpu.regs[DE].low);
				break;
			case 0x1C:
				rr_r8(&cpu.regs[HL].high);
				break;
			case 0x1D:
				rr_r8(&cpu.regs[HL].low);
				break;
			case 0x1E:
				rr_aHL();
				break;
			case 0x1F:
				rr_r8(&cpu.regs[AF].high);
				break;
			case 0x20:
				sla_r8(&cpu.regs[BC].high);
				break;
			case 0x21:
				sla_r8(&cpu.regs[BC].low);
				break;
			case 0x22:
				sla_r8(&cpu.regs[DE].high);
				break;
			case 0x23:
				sla_r8(&cpu.regs[DE].low);
				break;
			case 0x24:
				sla_r8(&cpu.regs[HL].high);
				break;
			case 0x25:
				sla_r8(&cpu.regs[HL].low);
				break;
			case 0x26:
				sla_aHL();
				break;
			case 0x27:
				sla_r8(&cpu.regs[AF].high);
				break;
			case 0x28:
				sra_r8(&cpu.regs[BC].high);
				break;
			case 0x29:
				sra_r8(&cpu.regs[BC].low);
				break;
			case 0x2A:
				sra_r8(&cpu.regs[DE].high);
				break;
			case 0x2B:
				sra_r8(&cpu.regs[DE].low);
				break;
			case 0x2C:
				sra_r8(&cpu.regs[HL].high);
				break;
			case 0x2D:
				sra_r8(&cpu.regs[HL].low);
				break;
			case 0x2E:
				sra_aHL();
				break;
			case 0x2F:
				sra_r8(&cpu.regs[AF].high);
				break;
			case 0x30:
				swap_r8(&cpu.regs[BC].high);
				break;
			case 0x31:
				swap_r8(&cpu.regs[BC].low);
				break;
			case 0x32:
				swap_r8(&cpu.regs[DE].high);
				break;
			case 0x33:
				swap_r8(&cpu.regs[DE].low);
				break;
			case 0x34:
				swap_r8(&cpu.regs[HL].high);
				break;
			case 0x35:
				swap_r8(&cpu.regs[HL].low);
				break;
			case 0x36:
				swap_aHL();
				break;
			case 0x37:
				swap_r8(&cpu.regs[AF].high);
				break;
			case 0x38:
				srl_r8(&cpu.regs[BC].high);
				break;
			case 0x39:
				srl_r8(&cpu.regs[BC].low);
				break;
			case 0x3A:
				srl_r8(&cpu.regs[DE].high);
				break;
			case 0x3B:
				srl_r8(&cpu.regs[DE].low);
				break;
			case 0x3C:
				srl_r8(&cpu.regs[HL].high);
				break;
			case 0x3D:
				srl_r8(&cpu.regs[HL].low);
				break;
			case 0x3E:
				srl_aHL();
				break;
			case 0x3F:
				srl_r8(&cpu.regs[AF].high);
				break;
			case 0x40:
				bit_u3_r8(0, cpu.regs[BC].high);
				break;
			case 0x41:
				bit_u3_r8(0, cpu.regs[BC].low);
				break;
			case 0x42:
				bit_u3_r8(0, cpu.regs[DE].high);
				break;
			case 0x43:
				bit_u3_r8(0, cpu.regs[DE].low);
				break;
			case 0x44:
				bit_u3_r8(0, cpu.regs[HL].high);
				break;
			case 0x45:
				bit_u3_r8(0, cpu.regs[HL].low);
				break;
			case 0x46:
				bit_u3_aHL(0);
				break;
			case 0x47:
				bit_u3_r8(0, cpu.regs[AF].high);
				break;
			case 0x48:
				bit_u3_r8(1, cpu.regs[BC].high);
				break;
			case 0x49:
				bit_u3_r8(1, cpu.regs[BC].low);
				break;
			case 0x4A:
				bit_u3_r8(1, cpu.regs[DE].high);
				break;
			case 0x4B:
				bit_u3_r8(1, cpu.regs[DE].low);
				break;
			case 0x4C:
				bit_u3_r8(1, cpu.regs[HL].high);
				break;
			case 0x4D:
				bit_u3_r8(1, cpu.regs[HL].low);
				break;
			case 0x4E:
				bit_u3_aHL(1);
				break;
			case 0x4F:
				bit_u3_r8(1, cpu.regs[AF].high);
				break;
			case 0x50:
				bit_u3_r8(2, cpu.regs[BC].high);
				break;
			case 0x51:
				bit_u3_r8(2, cpu.regs[BC].low);
				break;
			case 0x52:
				bit_u3_r8(2, cpu.regs[DE].high);
				break;
			case 0x53:
				bit_u3_r8(2, cpu.regs[DE].low);
				break;
			case 0x54:
				bit_u3_r8(2, cpu.regs[HL].high);
				break;
			case 0x55:
				bit_u3_r8(2, cpu.regs[HL].low);
				break;
			case 0x56:
				bit_u3_aHL(2);
				break;
			case 0x57:
				bit_u3_r8(2, cpu.regs[AF].high);
				break;
			case 0x58:
				bit_u3_r8(3, cpu.regs[BC].high);
				break;
			case 0x59:
				bit_u3_r8(3, cpu.regs[BC].low);
				break;
			case 0x5A:
				bit_u3_r8(3, cpu.regs[DE].high);
				break;
			case 0x5B:
				bit_u3_r8(3, cpu.regs[DE].low);
				break;
			case 0x5C:
				bit_u3_r8(3, cpu.regs[HL].high);
				break;
			case 0x5D:
				bit_u3_r8(3, cpu.regs[HL].low);
				break;
			case 0x5E:
				bit_u3_aHL(3);
				break;
			case 0x5F:
				bit_u3_r8(3, cpu.regs[AF].high);
				break;
			case 0x60:
				bit_u3_r8(4, cpu.regs[BC].high);
				break;
			case 0x61:
				bit_u3_r8(4, cpu.regs[BC].low);
				break;
			case 0x62:
				bit_u3_r8(4, cpu.regs[DE].high);
				break;
			case 0x63:
				bit_u3_r8(4, cpu.regs[DE].low);
				break;
			case 0x64:
				bit_u3_r8(4, cpu.regs[HL].high);
				break;
			case 0x65:
				bit_u3_r8(4, cpu.regs[HL].low);
				break;
			case 0x66:
				bit_u3_aHL(4);
				break;
			case 0x67:
				bit_u3_r8(4, cpu.regs[AF].high);
				break;
			case 0x68:
				bit_u3_r8(5, cpu.regs[BC].high);
				break;
			case 0x69:
				bit_u3_r8(5, cpu.regs[BC].low);
				break;
			case 0x6A:
				bit_u3_r8(5, cpu.regs[DE].high);
				break;
			case 0x6B:
				bit_u3_r8(5, cpu.regs[DE].low);
				break;
			case 0x6C:
				bit_u3_r8(5, cpu.regs[HL].high);
				break;
			case 0x6D:
				bit_u3_r8(5, cpu.regs[HL].low);
				break;
			case 0x6E:
				bit_u3_aHL(5);
				break;
			case 0x6F:
				bit_u3_r8(5, cpu.regs[AF].high);
				break;
			case 0x70:
				bit_u3_r8(6, cpu.regs[BC].high);
				break;
			case 0x71:
				bit_u3_r8(6, cpu.regs[BC].low);
				break;
			case 0x72:
				bit_u3_r8(6, cpu.regs[DE].high);
				break;
			case 0x73:
				bit_u3_r8(6, cpu.regs[DE].low);
				break;
			case 0x74:
				bit_u3_r8(6, cpu.regs[HL].high);
				break;
			case 0x75:
				bit_u3_r8(6, cpu.regs[HL].low);
				break;
			case 0x76:
				bit_u3_aHL(6);
				break;
			case 0x77:
				bit_u3_r8(6, cpu.regs[AF].high);
				break;
			case 0x78:
				bit_u3_r8(7, cpu.regs[BC].high);
				break;
			case 0x79:
				bit_u3_r8(7, cpu.regs[BC].low);
				break;
			case 0x7A:
				bit_u3_r8(7, cpu.regs[DE].high);
				break;
			case 0x7B:
				bit_u3_r8(7, cpu.regs[DE].low);
				break;
			case 0x7C:
				bit_u3_r8(7, cpu.regs[HL].high);
				break;
			case 0x7D:
				bit_u3_r8(7, cpu.regs[HL].low);
				break;
			case 0x7E:
				bit_u3_aHL(7);
				break;
			case 0x7F:
				bit_u3_r8(7, cpu.regs[AF].high);
				break;
			case 0x80:
				res_u3_r8(0, &cpu.regs[BC].high);
				break;
			case 0x81:
				res_u3_r8(0, &cpu.regs[BC].low);
				break;
			case 0x82:
				res_u3_r8(0, &cpu.regs[DE].high);
				break;
			case 0x83:
				res_u3_r8(0, &cpu.regs[DE].low);
				break;
			case 0x84:
				res_u3_r8(0, &cpu.regs[HL].high);
				break;
			case 0x85:
				res_u3_r8(0, &cpu.regs[HL].low);
				break;
			case 0x86:
				res_u3_aHL(0);
				break;
			case 0x87:
				res_u3_r8(0, &cpu.regs[AF].high);
				break;
			case 0x88:
				res_u3_r8(1, &cpu.regs[BC].high);
				break;
			case 0x89:
				res_u3_r8(1, &cpu.regs[BC].low);
				break;
			case 0x8A:
				res_u3_r8(1, &cpu.regs[DE].high);
				break;
			case 0x8B:
				res_u3_r8(1, &cpu.regs[DE].low);
				break;
			case 0x8C:
				res_u3_r8(1, &cpu.regs[HL].high);
				break;
			case 0x8D:
				res_u3_r8(1, &cpu.regs[HL].low);
				break;
			case 0x8E:
				res_u3_aHL(1);
				break;
			case 0x8F:
				res_u3_r8(1, &cpu.regs[AF].high);
				break;
			case 0x90:
				res_u3_r8(2, &cpu.regs[BC].high);
				break;
			case 0x91:
				res_u3_r8(2, &cpu.regs[BC].low);
				break;
			case 0x92:
				res_u3_r8(2, &cpu.regs[DE].high);
				break;
			case 0x93:
				res_u3_r8(2, &cpu.regs[DE].low);
				break;
			case 0x94:
				res_u3_r8(2, &cpu.regs[HL].high);
				break;
			case 0x95:
				res_u3_r8(2, &cpu.regs[HL].low);
				break;
			case 0x96:
				res_u3_aHL(2);
				break;
			case 0x97:
				res_u3_r8(2, &cpu.regs[AF].high);
				break;
			case 0x98:
				res_u3_r8(3, &cpu.regs[BC].high);
				break;
			case 0x99:
				res_u3_r8(3, &cpu.regs[BC].low);
				break;
			case 0x9A:
				res_u3_r8(3, &cpu.regs[DE].high);
				break;
			case 0x9B:
				res_u3_r8(3, &cpu.regs[DE].low);
				break;
			case 0x9C:
				res_u3_r8(3, &cpu.regs[HL].high);
				break;
			case 0x9D:
				res_u3_r8(3, &cpu.regs[HL].low);
				break;
			case 0x9E:
				res_u3_aHL(3);
				break;
			case 0x9F:
				res_u3_r8(3, &cpu.regs[AF].high);
				break;
			case 0xA0:
				res_u3_r8(4, &cpu.regs[BC].high);
				break;
			case 0xA1:
				res_u3_r8(4, &cpu.regs[BC].low);
				break;
			case 0xA2:
				res_u3_r8(4, &cpu.regs[DE].high);
				break;
			case 0xA3:
				res_u3_r8(4, &cpu.regs[DE].low);
				break;
			case 0xA4:
				res_u3_r8(4, &cpu.regs[HL].high);
				break;
			case 0xA5:
				res_u3_r8(4, &cpu.regs[HL].low);
				break;
			case 0xA6:
				res_u3_aHL(4);
				break;
			case 0xA7:
				res_u3_r8(4, &cpu.regs[AF].high);
				break;
			case 0xA8:
				res_u3_r8(5, &cpu.regs[BC].high);
				break;
			case 0xA9:
				res_u3_r8(5, &cpu.regs[BC].low);
				break;
			case 0xAA:
				res_u3_r8(5, &cpu.regs[DE].high);
				break;
			case 0xAB:
				res_u3_r8(5, &cpu.regs[DE].low);
				break;
			case 0xAC:
				res_u3_r8(5, &cpu.regs[HL].high);
				break;
			case 0xAD:
				res_u3_r8(5, &cpu.regs[HL].low);
				break;
			case 0xAE:
				res_u3_aHL(5);
				break;
			case 0xAF:
				res_u3_r8(5, &cpu.regs[AF].high);
				break;
			case 0xB0:
				res_u3_r8(6, &cpu.regs[BC].high);
				break;
			case 0xB1:
				res_u3_r8(6, &cpu.regs[BC].low);
				break;
			case 0xB2:
				res_u3_r8(6, &cpu.regs[DE].high);
				break;
			case 0xB3:
				res_u3_r8(6, &cpu.regs[DE].low);
				break;
			case 0xB4:
				res_u3_r8(6, &cpu.regs[HL].high);
				break;
			case 0xB5:
				res_u3_r8(6, &cpu.regs[HL].low);
				break;
			case 0xB6:
				res_u3_aHL(6);
				break;
			case 0xB7:
				res_u3_r8(6, &cpu.regs[AF].high);
				break;
			case 0xB8:
				res_u3_r8(7, &cpu.regs[BC].high);
				break;
			case 0xB9:
				res_u3_r8(7, &cpu.regs[BC].low);
				break;
			case 0xBA:
				res_u3_r8(7, &cpu.regs[DE].high);
				break;
			case 0xBB:
				res_u3_r8(7, &cpu.regs[DE].low);
				break;
			case 0xBC:
				res_u3_r8(7, &cpu.regs[HL].high);
				break;
			case 0xBD:
				res_u3_r8(7, &cpu.regs[HL].low);
				break;
			case 0xBE:
				res_u3_aHL(7);
				break;
			case 0xBF:
				res_u3_r8(7, &cpu.regs[AF].high);
				break;
			case 0xC0:
				set_u3_r8(0, &cpu.regs[BC].high);
				break;
			case 0xC1:
				set_u3_r8(0, &cpu.regs[BC].low);
				break;
			case 0xC2:
				set_u3_r8(0, &cpu.regs[DE].high);
				break;
			case 0xC3:
				set_u3_r8(0, &cpu.regs[DE].low);
				break;
			case 0xC4:
				set_u3_r8(0, &cpu.regs[HL].high);
				break;
			case 0xC5:
				set_u3_r8(0, &cpu.regs[HL].low);
				break;
			case 0xC6:
				set_u3_aHL(0);
				break;
			case 0xC7:
				set_u3_r8(0, &cpu.regs[AF].high);
				break;
			case 0xC8:
				set_u3_r8(1, &cpu.regs[BC].high);
				break;
			case 0xC9:
				set_u3_r8(1, &cpu.regs[BC].low);
				break;
			case 0xCA:
				set_u3_r8(1, &cpu.regs[DE].high);
				break;
			case 0xCB:
				set_u3_r8(1, &cpu.regs[DE].low);
				break;
			case 0xCC:
				set_u3_r8(1, &cpu.regs[HL].high);
				break;
			case 0xCD:
				set_u3_r8(1, &cpu.regs[HL].low);
				break;
			case 0xCE:
				set_u3_aHL(1);
				break;
			case 0xCF:
				set_u3_r8(1, &cpu.regs[AF].high);
				break;
			case 0xD0:
				set_u3_r8(2, &cpu.regs[BC].high);
				break;
			case 0xD1:
				set_u3_r8(2, &cpu.regs[BC].low);
				break;
			case 0xD2:
				set_u3_r8(2, &cpu.regs[DE].high);
				break;
			case 0xD3:
				set_u3_r8(2, &cpu.regs[DE].low);
				break;
			case 0xD4:
				set_u3_r8(2, &cpu.regs[HL].high);
				break;
			case 0xD5:
				set_u3_r8(2, &cpu.regs[HL].low);
				break;
			case 0xD6:
				set_u3_aHL(2);
				break;
			case 0xD7:
				set_u3_r8(2, &cpu.regs[AF].high);
				break;
			case 0xD8:
				set_u3_r8(3, &cpu.regs[BC].high);
				break;
			case 0xD9:
				set_u3_r8(3, &cpu.regs[BC].low);
				break;
			case 0xDA:
				set_u3_r8(3, &cpu.regs[DE].high);
				break;
			case 0xDB:
				set_u3_r8(3, &cpu.regs[DE].low);
				break;
			case 0xDC:
				set_u3_r8(3, &cpu.regs[HL].high);
				break;
			case 0xDD:
				set_u3_r8(3, &cpu.regs[HL].low);
				break;
			case 0xDE:
				set_u3_aHL(3);
				break;
			case 0xDF:
				set_u3_r8(3, &cpu.regs[AF].high);
				break;
			case 0xE0:
				set_u3_r8(4, &cpu.regs[BC].high);
				break;
			case 0xE1:
				set_u3_r8(4, &cpu.regs[BC].low);
				break;
			case 0xE2:
				set_u3_r8(4, &cpu.regs[DE].high);
				break;
			case 0xE3:
				set_u3_r8(4, &cpu.regs[DE].low);
				break;
			case 0xE4:
				set_u3_r8(4, &cpu.regs[HL].high);
				break;
			case 0xE5:
				set_u3_r8(4, &cpu.regs[HL].low);
				break;
			case 0xE6:
				set_u3_aHL(4);
				break;
			case 0xE7:
				set_u3_r8(4, &cpu.regs[AF].high);
				break;
			case 0xE8:
				set_u3_r8(5, &cpu.regs[BC].high);
				break;
			case 0xE9:
				set_u3_r8(5, &cpu.regs[BC].low);
				break;
			case 0xEA:
				set_u3_r8(5, &cpu.regs[DE].high);
				break;
			case 0xEB:
				set_u3_r8(5, &cpu.regs[DE].low);
				break;
			case 0xEC:
				set_u3_r8(5, &cpu.regs[HL].high);
				break;
			case 0xED:
				set_u3_r8(5, &cpu.regs[HL].low);
				break;
			case 0xEE:
				set_u3_aHL(5);
				break;
			case 0xEF:
				set_u3_r8(5, &cpu.regs[AF].high);
				break;
			case 0xF0:
				set_u3_r8(6, &cpu.regs[BC].high);
				break;
			case 0xF1:
				set_u3_r8(6, &cpu.regs[BC].low);
				break;
			case 0xF2:
				set_u3_r8(6, &cpu.regs[DE].high);
				break;
			case 0xF3:
				set_u3_r8(6, &cpu.regs[DE].low);
				break;
			case 0xF4:
				set_u3_r8(6, &cpu.regs[HL].high);
				break;
			case 0xF5:
				set_u3_r8(6, &cpu.regs[HL].low);
				break;
			case 0xF6:
				set_u3_aHL(6);
				break;
			case 0xF7:
				set_u3_r8(6, &cpu.regs[AF].high);
				break;
			case 0xF8:
				set_u3_r8(7, &cpu.regs[BC].high);
				break;
			case 0xF9:
				set_u3_r8(7, &cpu.regs[BC].low);
				break;
			case 0xFA:
				set_u3_r8(7, &cpu.regs[DE].high);
				break;
			case 0xFB:
				set_u3_r8(7, &cpu.regs[DE].low);
				break;
			case 0xFC:
				set_u3_r8(7, &cpu.regs[HL].high);
				break;
			case 0xFD:
				set_u3_r8(7, &cpu.regs[HL].low);
				break;
			case 0xFE:
				set_u3_aHL(7);
				break;
			case 0xFF:
				set_u3_r8(7, &cpu.regs[AF].high);
				break;
			}
			output_state[i].is_current_opcode_prefixed = false;
		} else {
			// ==================== NON-PREFIX ====================
			switch (byte) {
			case 0x00:
				nop();
				break;
			case 0x01: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				ld_r16_n16(&cpu.regs[BC].full, n16);
			} break;
			case 0x02:
				ld_a16_A(cpu.regs[BC].full);
				break;
			case 0x03:
				inc_r16(&cpu.regs[BC].full);
				break;
			case 0x04:
				inc_r8(&cpu.regs[BC].high);
				break;
			case 0x05:
				dec_r8(&cpu.regs[BC].high);
				break;
			case 0x06: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_r8_n8(&cpu.regs[BC].high, n8);
			} break;
			case 0x07:
				rlca();
				break;
			case 0x08: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				ld_addr16_SP(n16);
			} break;
			case 0x09:
				add_HL_r16(cpu.regs[BC].full);
				break;
			case 0x0A:
				ld_A_a16(cpu.regs[BC].full);
				break;
			case 0x0B:
				dec_r16(&cpu.regs[BC].full);
				break;
			case 0x0C:
				inc_r8(&cpu.regs[BC].low);
				break;
			case 0x0D:
				dec_r8(&cpu.regs[BC].low);
				break;
			case 0x0E: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_r8_n8(&cpu.regs[BC].low, n8);
			} break;
			case 0x0F:
				rrca();
				break;
			case 0x10: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				stop_n8(n8);
			} break;
			case 0x11: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				ld_r16_n16(&cpu.regs[DE].full, n16);
			} break;
			case 0x12:
				ld_a16_A(cpu.regs[DE].full);
				break;
			case 0x13:
				inc_r16(&cpu.regs[DE].full);
				break;
			case 0x14:
				inc_r8(&cpu.regs[DE].high);
				break;
			case 0x15:
				dec_r8(&cpu.regs[DE].high);
				break;
			case 0x16: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_r8_n8(&cpu.regs[DE].high, n8);
			} break;
			case 0x17:
				rla();
				break;
			case 0x18: {
				int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
				jr_n16(offset);
				break;
			}
			case 0x19:
				add_HL_r16(cpu.regs[DE].full);
				break;
			case 0x1A:
				ld_A_a16(cpu.regs[DE].full);
				break;
			case 0x1B:
				dec_r16(&cpu.regs[DE].full);
				break;
			case 0x1C:
				inc_r8(&cpu.regs[DE].low);
				break;
			case 0x1D:
				dec_r8(&cpu.regs[DE].low);
				break;
			case 0x1E: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_r8_n8(&cpu.regs[DE].low, n8);
			} break;
			case 0x1F:
				rra();
				break;
			case 0x20: {
				int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
				jr_cc_n16(Z, false, offset);
				break;
			}
			case 0x21: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				ld_r16_n16(&cpu.regs[HL].full, n16);
			} break;
			case 0x22:
				ld_aHLi_A();
				break;
			case 0x23:
				inc_r16(&cpu.regs[HL].full);
				break;
			case 0x24:
				inc_r8(&cpu.regs[HL].high);
				break;
			case 0x25:
				dec_r8(&cpu.regs[HL].high);
				break;
			case 0x26: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_r8_n8(&cpu.regs[HL].high, n8);
			} break;
			case 0x27:
				daa();
				break;
			case 0x28: {
				int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
				jr_cc_n16(Z, true, offset);
				break;
			}
			case 0x29:
				add_HL_r16(cpu.regs[HL].full);
				break;
			case 0x2A:
				ld_A_aHLi();
				break;
			case 0x2B:
				dec_r16(&cpu.regs[HL].full);
				break;
			case 0x2C:
				inc_r8(&cpu.regs[HL].low);
				break;
			case 0x2D:
				dec_r8(&cpu.regs[HL].low);
				break;
			case 0x2E: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_r8_n8(&cpu.regs[HL].low, n8);
			} break;
			case 0x2F:
				cpl();
				break;
			case 0x30: {
				int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
				jr_cc_n16(C, false, offset);
				break;
			}
			case 0x31: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				ld_r16_n16(&cpu.regs[SP].full, n16);
			} break;
			case 0x32:
				ld_aHLd_A();
				break;
			case 0x33:
				inc_r16(&cpu.regs[SP].full);
				break;
			case 0x34:
				inc_aHL();
				break;
			case 0x35:
				dec_aHL();
				break;
			case 0x36: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_aHL_n8(n8);
			} break;
			case 0x37:
				scf();
				break;
			case 0x38: {
				int8_t offset = (int8_t)read8(cpu.regs[PC].full + 1);
				jr_cc_n16(C, true, offset);
				break;
			}
			case 0x39:
				add_HL_r16(cpu.regs[SP].full);
				break;
			case 0x3A:
				ld_A_aHLd();
				break;
			case 0x3B:
				dec_r16(&cpu.regs[SP].full);
				break;
			case 0x3C:
				inc_r8(&cpu.regs[AF].high);
				break;
			case 0x3D:
				dec_r8(&cpu.regs[AF].high);
				break;
			case 0x3E: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ld_r8_n8(&cpu.regs[AF].high, n8);
			} break;
			case 0x3F:
				ccf();
				break;
			case 0x40:
				ld_r8_r8(&cpu.regs[BC].high, cpu.regs[BC].high);
				break;
			case 0x41:
				ld_r8_r8(&cpu.regs[BC].high, cpu.regs[BC].low);
				break;
			case 0x42:
				ld_r8_r8(&cpu.regs[BC].high, cpu.regs[DE].high);
				break;
			case 0x43:
				ld_r8_r8(&cpu.regs[BC].high, cpu.regs[DE].low);
				break;
			case 0x44:
				ld_r8_r8(&cpu.regs[BC].high, cpu.regs[HL].high);
				break;
			case 0x45:
				ld_r8_r8(&cpu.regs[BC].high, cpu.regs[HL].low);
				break;
			case 0x46:
				ld_r8_aHL(&cpu.regs[BC].high);
				break;
			case 0x47:
				ld_r8_r8(&cpu.regs[BC].high, cpu.regs[AF].high);
				break;
			case 0x48:
				ld_r8_r8(&cpu.regs[BC].low, cpu.regs[BC].high);
				break;
			case 0x49:
				ld_r8_r8(&cpu.regs[BC].low, cpu.regs[BC].low);
				break;
			case 0x4A:
				ld_r8_r8(&cpu.regs[BC].low, cpu.regs[DE].high);
				break;
			case 0x4B:
				ld_r8_r8(&cpu.regs[BC].low, cpu.regs[DE].low);
				break;
			case 0x4C:
				ld_r8_r8(&cpu.regs[BC].low, cpu.regs[HL].high);
				break;
			case 0x4D:
				ld_r8_r8(&cpu.regs[BC].low, cpu.regs[HL].low);
				break;
			case 0x4E:
				ld_r8_aHL(&cpu.regs[BC].low);
				break;
			case 0x4F:
				ld_r8_r8(&cpu.regs[BC].low, cpu.regs[AF].high);
				break;
			case 0x50:
				ld_r8_r8(&cpu.regs[DE].high, cpu.regs[BC].high);
				break;
			case 0x51:
				ld_r8_r8(&cpu.regs[DE].high, cpu.regs[BC].low);
				break;
			case 0x52:
				ld_r8_r8(&cpu.regs[DE].high, cpu.regs[DE].high);
				break;
			case 0x53:
				ld_r8_r8(&cpu.regs[DE].high, cpu.regs[DE].low);
				break;
			case 0x54:
				ld_r8_r8(&cpu.regs[DE].high, cpu.regs[HL].high);
				break;
			case 0x55:
				ld_r8_r8(&cpu.regs[DE].high, cpu.regs[HL].low);
				break;
			case 0x56:
				ld_r8_aHL(&cpu.regs[DE].high);
				break;
			case 0x57:
				ld_r8_r8(&cpu.regs[DE].high, cpu.regs[AF].high);
				break;
			case 0x58:
				ld_r8_r8(&cpu.regs[DE].low, cpu.regs[BC].high);
				break;
			case 0x59:
				ld_r8_r8(&cpu.regs[DE].low, cpu.regs[BC].low);
				break;
			case 0x5A:
				ld_r8_r8(&cpu.regs[DE].low, cpu.regs[DE].high);
				break;
			case 0x5B:
				ld_r8_r8(&cpu.regs[DE].low, cpu.regs[DE].low);
				break;
			case 0x5C:
				ld_r8_r8(&cpu.regs[DE].low, cpu.regs[HL].high);
				break;
			case 0x5D:
				ld_r8_r8(&cpu.regs[DE].low, cpu.regs[HL].low);
				break;
			case 0x5E:
				ld_r8_aHL(&cpu.regs[DE].low);
				break;
			case 0x5F:
				ld_r8_r8(&cpu.regs[DE].low, cpu.regs[AF].high);
				break;
			case 0x60:
				ld_r8_r8(&cpu.regs[HL].high, cpu.regs[BC].high);
				break;
			case 0x61:
				ld_r8_r8(&cpu.regs[HL].high, cpu.regs[BC].low);
				break;
			case 0x62:
				ld_r8_r8(&cpu.regs[HL].high, cpu.regs[DE].high);
				break;
			case 0x63:
				ld_r8_r8(&cpu.regs[HL].high, cpu.regs[DE].low);
				break;
			case 0x64:
				ld_r8_r8(&cpu.regs[HL].high, cpu.regs[HL].high);
				break;
			case 0x65:
				ld_r8_r8(&cpu.regs[HL].high, cpu.regs[HL].low);
				break;
			case 0x66:
				ld_r8_aHL(&cpu.regs[HL].high);
				break;
			case 0x67:
				ld_r8_r8(&cpu.regs[HL].high, cpu.regs[AF].high);
				break;
			case 0x68:
				ld_r8_r8(&cpu.regs[HL].low, cpu.regs[BC].high);
				break;
			case 0x69:
				ld_r8_r8(&cpu.regs[HL].low, cpu.regs[BC].low);
				break;
			case 0x6A:
				ld_r8_r8(&cpu.regs[HL].low, cpu.regs[DE].high);
				break;
			case 0x6B:
				ld_r8_r8(&cpu.regs[HL].low, cpu.regs[DE].low);
				break;
			case 0x6C:
				ld_r8_r8(&cpu.regs[HL].low, cpu.regs[HL].high);
				break;
			case 0x6D:
				ld_r8_r8(&cpu.regs[HL].low, cpu.regs[HL].low);
				break;
			case 0x6E:
				ld_r8_aHL(&cpu.regs[HL].low);
				break;
			case 0x6F:
				ld_r8_r8(&cpu.regs[HL].low, cpu.regs[AF].high);
				break;
			case 0x70:
				ld_aHL_r8(cpu.regs[BC].high);
				break;
			case 0x71:
				ld_aHL_r8(cpu.regs[BC].low);
				break;
			case 0x72:
				ld_aHL_r8(cpu.regs[DE].high);
				break;
			case 0x73:
				ld_aHL_r8(cpu.regs[DE].low);
				break;
			case 0x74:
				ld_aHL_r8(cpu.regs[HL].high);
				break;
			case 0x75:
				ld_aHL_r8(cpu.regs[HL].low);
				break;
			case 0x76:
				halt();
				break;
			case 0x77:
				ld_aHL_r8(cpu.regs[AF].high);
				break;
			case 0x78:
				ld_r8_r8(&cpu.regs[AF].high, cpu.regs[BC].high);
				break;
			case 0x79:
				ld_r8_r8(&cpu.regs[AF].high, cpu.regs[BC].low);
				break;
			case 0x7A:
				ld_r8_r8(&cpu.regs[AF].high, cpu.regs[DE].high);
				break;
			case 0x7B:
				ld_r8_r8(&cpu.regs[AF].high, cpu.regs[DE].low);
				break;
			case 0x7C:
				ld_r8_r8(&cpu.regs[AF].high, cpu.regs[HL].high);
				break;
			case 0x7D:
				ld_r8_r8(&cpu.regs[AF].high, cpu.regs[HL].low);
				break;
			case 0x7E:
				ld_r8_aHL(&cpu.regs[AF].high);
				break;
			case 0x7F:
				ld_r8_r8(&cpu.regs[AF].high, cpu.regs[AF].high);
				break;
			case 0x80:
				add_A_r8(cpu.regs[BC].high);
				break;
			case 0x81:
				add_A_r8(cpu.regs[BC].low);
				break;
			case 0x82:
				add_A_r8(cpu.regs[DE].high);
				break;
			case 0x83:
				add_A_r8(cpu.regs[DE].low);
				break;
			case 0x84:
				add_A_r8(cpu.regs[HL].high);
				break;
			case 0x85:
				add_A_r8(cpu.regs[HL].low);
				break;
			case 0x86:
				add_A_aHL();
				break;
			case 0x87:
				add_A_r8(cpu.regs[AF].high);
				break;
			case 0x88:
				adc_A_r8(cpu.regs[BC].high);
				break;
			case 0x89:
				adc_A_r8(cpu.regs[BC].low);
				break;
			case 0x8A:
				adc_A_r8(cpu.regs[DE].high);
				break;
			case 0x8B:
				adc_A_r8(cpu.regs[DE].low);
				break;
			case 0x8C:
				adc_A_r8(cpu.regs[HL].high);
				break;
			case 0x8D:
				adc_A_r8(cpu.regs[HL].low);
				break;
			case 0x8E:
				adc_A_aHL();
				break;
			case 0x8F:
				adc_A_r8(cpu.regs[AF].high);
				break;
			case 0x90:
				sub_A_r8(cpu.regs[BC].high);
				break;
			case 0x91:
				sub_A_r8(cpu.regs[BC].low);
				break;
			case 0x92:
				sub_A_r8(cpu.regs[DE].high);
				break;
			case 0x93:
				sub_A_r8(cpu.regs[DE].low);
				break;
			case 0x94:
				sub_A_r8(cpu.regs[HL].high);
				break;
			case 0x95:
				sub_A_r8(cpu.regs[HL].low);
				break;
			case 0x96:
				sub_A_aHL();
				break;
			case 0x97:
				sub_A_r8(cpu.regs[AF].high);
				break;
			case 0x98:
				sbc_A_r8(cpu.regs[BC].high);
				break;
			case 0x99:
				sbc_A_r8(cpu.regs[BC].low);
				break;
			case 0x9A:
				sbc_A_r8(cpu.regs[DE].high);
				break;
			case 0x9B:
				sbc_A_r8(cpu.regs[DE].low);
				break;
			case 0x9C:
				sbc_A_r8(cpu.regs[HL].high);
				break;
			case 0x9D:
				sbc_A_r8(cpu.regs[HL].low);
				break;
			case 0x9E:
				sbc_A_aHL();
				break;
			case 0x9F:
				sbc_A_r8(cpu.regs[AF].high);
				break;
			case 0xA0:
				and_A_r8(cpu.regs[BC].high);
				break;
			case 0xA1:
				and_A_r8(cpu.regs[BC].low);
				break;
			case 0xA2:
				and_A_r8(cpu.regs[DE].high);
				break;
			case 0xA3:
				and_A_r8(cpu.regs[DE].low);
				break;
			case 0xA4:
				and_A_r8(cpu.regs[HL].high);
				break;
			case 0xA5:
				and_A_r8(cpu.regs[HL].low);
				break;
			case 0xA6:
				and_A_aHL();
				break;
			case 0xA7:
				and_A_r8(cpu.regs[AF].high);
				break;
			case 0xA8:
				xor_A_r8(cpu.regs[BC].high);
				break;
			case 0xA9:
				xor_A_r8(cpu.regs[BC].low);
				break;
			case 0xAA:
				xor_A_r8(cpu.regs[DE].high);
				break;
			case 0xAB:
				xor_A_r8(cpu.regs[DE].low);
				break;
			case 0xAC:
				xor_A_r8(cpu.regs[HL].high);
				break;
			case 0xAD:
				xor_A_r8(cpu.regs[HL].low);
				break;
			case 0xAE:
				xor_A_aHL();
				break;
			case 0xAF:
				xor_A_r8(cpu.regs[AF].high);
				break;
			case 0xB0:
				or_A_r8(cpu.regs[BC].high);
				break;
			case 0xB1:
				or_A_r8(cpu.regs[BC].low);
				break;
			case 0xB2:
				or_A_r8(cpu.regs[DE].high);
				break;
			case 0xB3:
				or_A_r8(cpu.regs[DE].low);
				break;
			case 0xB4:
				or_A_r8(cpu.regs[HL].high);
				break;
			case 0xB5:
				or_A_r8(cpu.regs[HL].low);
				break;
			case 0xB6:
				or_A_aHL();
				break;
			case 0xB7:
				or_A_r8(cpu.regs[AF].high);
				break;
			case 0xB8:
				cp_A_r8(cpu.regs[BC].high);
				break;
			case 0xB9:
				cp_A_r8(cpu.regs[BC].low);
				break;
			case 0xBA:
				cp_A_r8(cpu.regs[DE].high);
				break;
			case 0xBB:
				cp_A_r8(cpu.regs[DE].low);
				break;
			case 0xBC:
				cp_A_r8(cpu.regs[HL].high);
				break;
			case 0xBD:
				cp_A_r8(cpu.regs[HL].low);
				break;
			case 0xBE:
				cp_A_aHL();
				break;
			case 0xBF:
				cp_A_r8(cpu.regs[AF].high);
				break;
			case 0xC0:
				ret_cc(Z, false);
				break;
			case 0xC1:
				pop_r16(&cpu.regs[BC].full);
				break;
			case 0xC2: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				jp_cc_n16(Z, false, n16);
				break;
			}
			case 0xC3: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				jp_n16(n16);
				break;
			}
			case 0xC4: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				call_cc_n16(Z, false, n16);
				break;
			}
			case 0xC5:
				push_r16(cpu.regs[BC].full);
				break;
			case 0xC6: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				add_A_n8(n8);
				break;
			}
			case 0xC7:
				rst(0x00);
				break;
			case 0xC8:
				ret_cc(Z, true);
				break;
			case 0xC9:
				ret();
				break;
			case 0xCA: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				jp_cc_n16(Z, true, n16);
				break;
			}
			case 0xCB:
				output_state[i].is_current_opcode_prefixed = true;
				cpu.regs[PC].full += 1;
				break;
			case 0xCC: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				call_cc_n16(Z, true, n16);
				break;
			}
			case 0xCD: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				call_n16(n16);
				break;
			}
			case 0xCE: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				adc_A_n8(n8);
				break;
			}
			case 0xCF:
				rst(0x08);
				break;
			case 0xD0:
				ret_cc(C, false);
				break;
			case 0xD1:
				pop_r16(&cpu.regs[DE].full);
				break;
			case 0xD2: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				jp_cc_n16(C, false, n16);
				break;
			}
			case 0xD4: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				call_cc_n16(C, false, n16);
				break;
			}
			case 0xD5:
				push_r16(cpu.regs[DE].full);
				break;
			case 0xD6: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				sub_A_n8(n8);
				break;
			}
			case 0xD7:
				rst(0x10);
				break;
			case 0xD8:
				ret_cc(C, true);
				break;
			case 0xD9:
				reti();
				break;
			case 0xDA: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				jp_cc_n16(C, true, n16);
				break;
			}
			case 0xDC: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				call_cc_n16(C, true, n16);
				break;
			}
			case 0xDE: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				sbc_A_n8(n8);
				break;
			}
			case 0xDF:
				rst(0x18);
				break;
			case 0xE0: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ldh_addr16_A(0xFF00 + n8);
				break;
			}
			case 0xE1:
				pop_r16(&cpu.regs[HL].full);
				break;
			case 0xE2:
				ldh_aC_A();
				break;
			case 0xE5:
				push_r16(cpu.regs[HL].full);
				break;
			case 0xE6: {
				int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
				and_A_n8(n8);
			} break;
			case 0xE7:
				rst(0x20);
				break;
			case 0xE8: {
				int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
				add_SP_n8(n8);
			} break;
			case 0xE9:
				jp_aHL();
				break;
			case 0xEA: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				ld_addr16_A(n16);
				break;
			}
			case 0xEE: {
				int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
				xor_A_n8(n8);
			} break;
			case 0xEF:
				rst(0x28);
				break;
			case 0xF0: {
				uint8_t n8 = read8(cpu.regs[PC].full + 1);
				ldh_A_addr16(0xFF00 + n8);
				break;
			}
			case 0xF1:
				pop_AF();
				break;
			case 0xF2:
				ldh_A_aC();
				break;
			case 0xF3:
				di();
				break;
			case 0xF5:
				push_AF();
				break;
			case 0xF6: {
				int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
				or_A_n8(n8);
			} break;
			case 0xF7:
				rst(0x30);
				break;
			case 0xF8: {
				int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
				ld_HL_SPe8(n8);
			} break;
			case 0xF9:
				ld_SP_HL();
				break;
			case 0xFA: {
				uint16_t n16 = read16(cpu.regs[PC].full + 1);
				ld_A_addr16(n16);
				break;
			}
			case 0xFB:
				ei();
				break;
			case 0xFE: {
				int8_t n8 = (int8_t)read8(cpu.regs[PC].full + 1);
				cp_A_n8(n8);
			} break;
			case 0xFF:
				rst(0x38);
				break;
			default:
				cpu.regs[PC].full += 1;
				cpu.cycle += 1;
				break;
			}
		}
	}
}
