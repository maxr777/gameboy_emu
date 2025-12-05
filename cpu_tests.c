#include "cJSON/cJSON.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;

typedef struct {
	char name[10];
	u8 a;
	u8 b;
	u8 c;
	u8 d;
	u8 e;
	u8 f;
	u8 h;
	u8 l;
	u16 pc;
	u16 sp;
	int memory_entries;
	u16 *memory_addresses;
	u8 *memory_values;
} test_state;

void print_test_state(test_state *test) {
	printf("a: %d\nb: %d\nc: %d\nd: %d\ne: %d\nf: %d\nh: %d\nl: %d\npc: %d\nsp: %d\n",
		   test->a, test->b, test->c, test->d, test->e, test->f, test->h, test->l, test->pc, test->sp);

	for (int i = 0; i < test->memory_entries; ++i) {
		printf("address: %d\nvalue: %d\n", test->memory_addresses[i], test->memory_values[i]);
	}
}

int main() {
	for (int instruction_opcode = 0xFF; instruction_opcode < (0xFF + 1); ++instruction_opcode) {
		char file_name[100];
		snprintf(file_name, sizeof(file_name), "sm83-tests-v2/%02x.json", instruction_opcode);
		FILE *f = fopen(file_name, "rb");
		if (f == 0) {
			fprintf(stderr, "%s - ", file_name);
			perror("fopen() failed");
			continue;
		}
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *buffer = malloc(fsize + 1);
		if (buffer == 0) {
			perror("malloc() failed:");
			return 1;
		}
		fread(buffer, 1, fsize, f);
		fclose(f);

		buffer[fsize] = '\0';

		cJSON *json = cJSON_Parse(buffer);
		free(buffer);
		if (json == 0) {
			const char *error_ptr = cJSON_GetErrorPtr();
			if (error_ptr != NULL) {
				fprintf(stderr, "Error: JSON parse failed before: %s\n", error_ptr);
			}
			return 1;
		}

		int test_num = cJSON_GetArraySize(json);

		for (int i = 0; i < test_num; ++i) {
			const cJSON *item = cJSON_GetArrayItem(json, i);
			if (item == 0) {
				fprintf(stderr, "cJSON_GetArrayItem() failed\n");
				return 1;
			}

			cJSON *name = cJSON_GetObjectItem(item, "name");
			char *name_str = cJSON_GetStringValue(name);

			printf("test name: %s\n", name_str);

			test_state current_test_initial = {};

			{
				printf("initial:\n");
				cJSON *initial = cJSON_GetObjectItem(item, "initial");

				cJSON *a = cJSON_GetObjectItem(initial, "a");
				current_test_initial.a = (u8)cJSON_GetNumberValue(a);

				cJSON *b = cJSON_GetObjectItem(initial, "b");
				current_test_initial.a = (u8)cJSON_GetNumberValue(b);

				cJSON *c = cJSON_GetObjectItem(initial, "c");
				current_test_initial.a = (u8)cJSON_GetNumberValue(c);

				cJSON *d = cJSON_GetObjectItem(initial, "d");
				current_test_initial.d = (u8)cJSON_GetNumberValue(d);

				cJSON *e = cJSON_GetObjectItem(initial, "e");
				current_test_initial.e = (u8)cJSON_GetNumberValue(e);

				cJSON *f = cJSON_GetObjectItem(initial, "f");
				current_test_initial.f = (u8)cJSON_GetNumberValue(f);

				cJSON *h = cJSON_GetObjectItem(initial, "h");
				current_test_initial.h = (u8)cJSON_GetNumberValue(h);

				cJSON *l = cJSON_GetObjectItem(initial, "l");
				current_test_initial.l = (u8)cJSON_GetNumberValue(l);

				cJSON *pc = cJSON_GetObjectItem(initial, "pc");
				current_test_initial.pc = (u16)cJSON_GetNumberValue(pc);

				cJSON *sp = cJSON_GetObjectItem(initial, "sp");
				current_test_initial.sp = (u16)cJSON_GetNumberValue(sp);

				cJSON *ram = cJSON_GetObjectItem(initial, "ram");
				current_test_initial.memory_entries = cJSON_GetArraySize(ram);
				current_test_initial.memory_addresses = malloc(sizeof(u16) * current_test_initial.memory_entries);
				current_test_initial.memory_values = malloc(sizeof(u8) * current_test_initial.memory_entries);

				for (int j = 0; j < current_test_initial.memory_entries; ++j) {
					cJSON *ram_entry = cJSON_GetArrayItem(ram, j);

					cJSON *ram_address = cJSON_GetArrayItem(ram_entry, 0);
					current_test_initial.memory_addresses[j] = (u16)cJSON_GetNumberValue(ram_address);

					cJSON *ram_value = cJSON_GetArrayItem(ram_entry, 1);
					current_test_initial.memory_values[j] = (u16)cJSON_GetNumberValue(ram_value);
				}

				print_test_state(&current_test_initial);
			}

			test_state current_test_final = {};

			{
				printf("final:\n");
				cJSON *final = cJSON_GetObjectItem(item, "final");

				cJSON *a = cJSON_GetObjectItem(final, "a");
				current_test_final.a = (u8)cJSON_GetNumberValue(a);

				cJSON *b = cJSON_GetObjectItem(final, "b");
				current_test_final.b = (u8)cJSON_GetNumberValue(b);

				cJSON *c = cJSON_GetObjectItem(final, "c");
				current_test_final.c = (u8)cJSON_GetNumberValue(c);

				cJSON *d = cJSON_GetObjectItem(final, "d");
				current_test_final.d = (u8)cJSON_GetNumberValue(d);

				cJSON *e = cJSON_GetObjectItem(final, "e");
				current_test_final.e = (u8)cJSON_GetNumberValue(e);

				cJSON *f = cJSON_GetObjectItem(final, "f");
				current_test_final.f = (u8)cJSON_GetNumberValue(f);

				cJSON *h = cJSON_GetObjectItem(final, "h");
				current_test_final.h = (u8)cJSON_GetNumberValue(h);

				cJSON *l = cJSON_GetObjectItem(final, "l");
				current_test_final.l = (u8)cJSON_GetNumberValue(l);

				cJSON *pc = cJSON_GetObjectItem(final, "pc");
				current_test_final.pc = (u16)cJSON_GetNumberValue(pc);

				cJSON *sp = cJSON_GetObjectItem(final, "sp");
				current_test_final.sp = (u16)cJSON_GetNumberValue(sp);

				cJSON *ram = cJSON_GetObjectItem(final, "ram");
				current_test_final.memory_entries = cJSON_GetArraySize(ram);

				current_test_final.memory_addresses = malloc(sizeof(u16) * current_test_final.memory_entries);
				current_test_final.memory_values = malloc(sizeof(u8) * current_test_final.memory_entries);

				for (int j = 0; j < current_test_final.memory_entries; ++j) {
					cJSON *ram_entry = cJSON_GetArrayItem(ram, j);

					cJSON *ram_address = cJSON_GetArrayItem(ram_entry, 0);
					current_test_final.memory_addresses[j] = (u16)cJSON_GetNumberValue(ram_address);

					cJSON *ram_value = cJSON_GetArrayItem(ram_entry, 1);
					current_test_final.memory_values[j] = (u16)cJSON_GetNumberValue(ram_value);
				}

				print_test_state(&current_test_final);
				printf("\n=============================================\n\n");

				free(current_test_initial.memory_addresses);
				free(current_test_initial.memory_values);
				free(current_test_final.memory_addresses);
				free(current_test_final.memory_values);
			}
		}
	}
}
