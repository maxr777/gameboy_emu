#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ================ MEMORY MAP ================

#define ROM_BANK_0_ADDR 0x0000
#define ROM_BANK_N_ADDR 0x4000
#define VRAM_ADDR       0x8000
#define EXTERN_RAM_ADDR 0xA000
#define WRAM_0_ADDR     0xC000
#define WRAM_N_ADDR     0xD000 // DMG = extension of WRAM_0, CGB = switchable bank 1-7
#define ECHO_RAM_ADDR   0xE000 // Prohibited, mirror of both WRAMs
#define OAM_ADDR        0xFE00
#define INVAL_MEM_ADDR  0xFEA0 // Prohibited
#define IO_REGS_ADDR    0xFF00
#define HRAM_ADDR       0xFF80
#define INT_ENABLE_ADDR 0xFFFF

// ================ IO REGISTERS ================

// joypad input
#define JOYPAD_INPUT 0xFF00

// serial transfer
#define SERIAL_TRANSFER 0xFF01
#define SERIAL_CONTROL  0xFF02

// timer and divider
#define DIVIDER_CONTROL 0xFF04
#define TIMER_COUNTER   0xFF05
#define TIMER_MOD       0xFF06
#define TIMER_CONTROL   0xFF07

// interrputs
#define INTERRUPT_FLAG 0xFF0F

// audio
#define CH1_SWEEP       0xFF10
#define CH1_TIMER       0xFF11
#define CH1_VOLUME      0xFF12
#define CH1_PERIOD_LOW  0xFF13
#define CH1_PERIOD_HIGH 0xFF14
#define CH2_TIMER       0xFF16
#define CH2_VOLUME      0xFF17
#define CH2_PERIOD_LOW  0xFF18
#define CH2_PERIOD_HIGH 0xFF19
#define CH3_DAC_ENABLE  0xFF1A
#define CH3_LEN_TIMER   0xFF1B
#define CH3_OUTPUT_LVL  0xFF1C
#define CH3_PERIOD_LOW  0xFF1D
#define CH3_PERIOD_HIGH 0xFF1E
#define CH4_LEN_TIMER   0xFF20
#define CH4_VOLUME      0xFF21
#define CH4_FREQUENCY   0xFF22
#define CH4_CONTROL     0xFF23
#define MASTER_VOLUME   0xFF24
#define SOUND_PANNING   0xFF25
#define AUDIO_CONTROL   0xFF26

// wave pattern
#define WAVE_PATTERN_RAM 0xFF30

// lcd control, status, position, scrolling, palettes
#define LCD_CONTROL       0xFF40
#define LCD_STATUS        0xFF41
#define BG_VIEWPORT_Y     0xFF42
#define BG_VIEWPORT_X     0xFF43
#define LCD_Y             0xFF44
#define LCD_LY_CMP        0xFF45
#define BG_PALETTE_DATA   0xFF47
#define OBJ_PALETTE0_DATA 0xFF48
#define OBJ_PALETTE1_DATA 0xFF49
#define WINDOW_Y7         0xFF4A
#define WINDOW_X7         0xFF4B

// vram bank select
#define VRAM_BANK 0xFF4F

// boot rom mapping control
#define BOOT_ROM_DISABLE 0xFF50

// vram dma
#define VRAM_DMA_SRC_HIGH  0xFF51
#define VRAM_DMA_SRC_LOW   0xFF52
#define VRAM_DMA_DEST_HIGH 0xFF53
#define VRAM_DMA_DEST_LOW  0xFF54
#define VRAM_DMA_START     0xFF55

// bg/obj palettes
#define CGB_BG_PALETTE_SPEC        0xFF68
#define CGB_BG_PALETTE_DATA        0xFF69
#define CGB_OBJ_COLOR_PALETTE_SPEC 0xFF6A
#define CGB_OBJ_COLOR_PALETTE_DATA 0xFF6B

// wram bank select
#define WRAM_BANK 0xFF70

// ================ HARDWARE ================

enum RegisterNames {
  AF,
  BC,
  DE,
  HL,
  SP, // always accessed as 16-bits
  PC, // always accessed as 16-bits
  REGISTER_COUNT
};

enum Flags {
  Z, // zero flag
  N, // subtract flag
  H, // half carry flag
  C, // carry flag
  FLAG_COUNT
};

// BC = Full 16 bit register
// B = High 8 bits
// C = Low 8 bits
typedef union {
  uint16_t full;
  struct {
    uint8_t low;
    uint8_t high;
  };
} Register;

typedef struct {
  uint8_t entry_point[4];
  uint8_t nintendo_logo[48];
  char title[16]; // already includes manufacturer code
  uint8_t cgb_flag;
  uint16_t new_license_code;
  uint8_t sgb_flag;
  uint8_t cartridge_type;
  uint8_t rom_size;
  uint8_t ram_size;
  uint8_t destination_code;
  uint8_t old_licensee_code;
  uint8_t mask_rom_version_number;
  uint8_t header_checksum;
  uint16_t global_checksum;
} CartridgeHeader;

extern Register regs[REGISTER_COUNT];
extern bool display[160][144];
extern uint8_t ram[8192];
extern uint8_t vram[8192];
extern uint8_t io_registers[128];
extern uint8_t oam[160];
extern CartridgeHeader cartridge_header;
extern int cycle;
extern bool prefix;

// ================ ROM STUFF ================

extern uint8_t *game_rom;
extern size_t game_size;
extern int current_rom_bank;
extern uint8_t boot_rom[256];

void rom_write(const uint16_t addr, const uint8_t val);
void mcb1_write(const uint16_t addr, const uint8_t val);

uint8_t rom_read(const uint16_t addr);
uint8_t mcb1_read(const uint16_t addr);

// ================ INTERRUPT STUFF ================

extern bool ime;
extern int ime_enable_counter;

// ================ HELPER FUNCTIONS ================

bool get_flag(const int flag);
void set_flag(const int flag, const bool value);

void write16(const uint16_t addr, const uint16_t val);
void write8(const uint16_t addr, const uint8_t val);
uint16_t read16(const uint16_t addr);
uint8_t read8(const uint16_t addr);

// ================ OPCODES ================

// LOADS
void ld_r8_r8(uint8_t *dest, const uint8_t src);
void ld_r8_n8(uint8_t *dest, const uint8_t val);
void ld_r16_n16(uint16_t *dest, const uint16_t val);
void ld_aHL_r8(const uint8_t src);
void ld_aHL_n8(const uint8_t val);
void ld_r8_aHL(uint8_t *dest);
void ld_a16_A(const uint16_t addr);
void ld_addr16_A(const uint16_t addr);
void ldh_addr16_A(const uint16_t addr);
void ldh_aC_A();
void ld_A_a16(const uint16_t addr);
void ld_A_addr16(const uint16_t addr);
void ldh_A_addr16(const uint16_t addr);
void ldh_A_aC();
void ld_aHLi_A();
void ld_aHLd_A();
void ld_A_aHLi();
void ld_A_aHLd();

// 8-BIT ARITHMETIC
void add_A_r8(const uint8_t src);
void add_A_aHL();
void adc_A_r8(const uint8_t src);
void adc_A_aHL();
void sub_A_r8(const uint8_t src);
void sbc_A_r8(const uint8_t src);
void dec_r8(uint8_t *dest);
void dec_aHL();
void inc_r8(uint8_t *dest);
void inc_aHL();
void cp_A_r8(const uint8_t src);

// 16-BIT ARITHMETIC
void add_HL_r16(const uint16_t src);
void dec_r16(uint16_t *dest);
void inc_r16(uint16_t *dest);

// BITWISE LOGIC
void and_A_r8(const uint8_t src);
void or_A_r8(const uint8_t src);
void cpl();
void xor_A_r8(const uint8_t src);

// BIT FLAGS
void bit_u3_r8(const int bit_num, const uint8_t src);
void bit_u3_aHL(const int bit_num);

// BIT SHIFTS
void rla();
void rlca();
void rra();
void rrca();

// JUMPS
void call_n16(const uint16_t addr);
void call_cc_n16(const int flag, const bool flag_state, const uint16_t addr);
void jp_n16(const uint16_t addr);
void jp_cc_n16(const int flag, const bool flag_state, const uint16_t addr);
void jp_aHL();
void jr_n16(const int8_t offset);
void jr_cc_n16(const int flag, const bool flag_state, const int8_t offset);
void rst(const uint8_t vec);
void ret();
void ret_cc(const int flag, const bool flag_state);
void reti();

// CARRY FLAG INSTRUCTIONS
void ccf();
void scf();

// STACK INSTRUCTIONS
void pop_r16(uint16_t *src);
void push_r16(const uint16_t src);
void ld_addr16_SP(const uint16_t addr);
void ld_HL_SPe8(const int8_t val);
void ld_SP_HL();
void add_SP_n8(const int8_t val);

// INTERRUPTS
void di();
void ei();
void halt();

// MISC
void daa();
void nop();

#endif
