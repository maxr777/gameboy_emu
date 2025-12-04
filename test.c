#include "cJSON.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;

typedef struct {
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

int main() {
	FILE *f = fopen("00.json", "rb");
	if (f == 0) {
		perror("fopen() failed:");
		return 1;
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
		const cJSON *item = cJSON_GetArrayItem(json, 0);
		if (item == 0) {
			fprintf(stderr, "cJSON_GetArrayItem() failed\n");
			return 1;
		}

		cJSON *name = cJSON_GetObjectItem(item, "name");
		char *name_str = cJSON_GetStringValue(name);
		printf("test name: %s\n", name_str);

		{
			printf("initial:\n");
			cJSON *initial = cJSON_GetObjectItem(item, "initial");

			cJSON *a = cJSON_GetObjectItem(initial, "a");
			u8 a_val = (u8)cJSON_GetNumberValue(a);

			cJSON *b = cJSON_GetObjectItem(initial, "b");
			u8 b_val = (u8)cJSON_GetNumberValue(b);

			cJSON *c = cJSON_GetObjectItem(initial, "c");
			u8 c_val = (u8)cJSON_GetNumberValue(c);

			cJSON *d = cJSON_GetObjectItem(initial, "d");
			u8 d_val = (u8)cJSON_GetNumberValue(d);

			cJSON *e = cJSON_GetObjectItem(initial, "e");
			u8 e_val = (u8)cJSON_GetNumberValue(e);

			cJSON *f = cJSON_GetObjectItem(initial, "f");
			u8 f_val = (u8)cJSON_GetNumberValue(f);

			cJSON *h = cJSON_GetObjectItem(initial, "h");
			u8 h_val = (u8)cJSON_GetNumberValue(h);

			cJSON *l = cJSON_GetObjectItem(initial, "l");
			u8 l_val = (u8)cJSON_GetNumberValue(l);

			cJSON *pc = cJSON_GetObjectItem(initial, "pc");
			u16 pc_val = (u16)cJSON_GetNumberValue(pc);

			cJSON *sp = cJSON_GetObjectItem(initial, "sp");
			u16 sp_val = (u16)cJSON_GetNumberValue(sp);

			printf("a: %d\nb: %d\nc: %d\nd: %d\ne: %d\nf: %d\nh: %d\nl: %d\npc: %d\nsp: %d\n",
				   a_val, b_val, c_val, d_val, e_val, f_val, h_val, l_val, pc_val, sp_val);

			cJSON *ram = cJSON_GetObjectItem(initial, "ram");
			int ram_size = cJSON_GetArraySize(ram);

			for (int j = 0; j < ram_size; ++j) {
				cJSON *ram_entry = cJSON_GetArrayItem(ram, j);

				cJSON *ram_address = cJSON_GetArrayItem(ram_entry, 0);
				u16 ram_address_num = (u16)cJSON_GetNumberValue(ram_address);

				cJSON *ram_value = cJSON_GetArrayItem(ram_entry, 1);
				u16 ram_value_num = (u16)cJSON_GetNumberValue(ram_value);

				printf("ram:\naddress:%d\nvalue:%d\n", ram_address_num, ram_value_num);
			}
		}

		{
			printf("final:\n");
			cJSON *final = cJSON_GetObjectItem(item, "final");

			cJSON *a = cJSON_GetObjectItem(final, "a");
			u8 a_val = (u8)cJSON_GetNumberValue(a);

			cJSON *b = cJSON_GetObjectItem(final, "b");
			u8 b_val = (u8)cJSON_GetNumberValue(b);

			cJSON *c = cJSON_GetObjectItem(final, "c");
			u8 c_val = (u8)cJSON_GetNumberValue(c);

			cJSON *d = cJSON_GetObjectItem(final, "d");
			u8 d_val = (u8)cJSON_GetNumberValue(d);

			cJSON *e = cJSON_GetObjectItem(final, "e");
			u8 e_val = (u8)cJSON_GetNumberValue(e);

			cJSON *f = cJSON_GetObjectItem(final, "f");
			u8 f_val = (u8)cJSON_GetNumberValue(f);

			cJSON *h = cJSON_GetObjectItem(final, "h");
			u8 h_val = (u8)cJSON_GetNumberValue(h);

			cJSON *l = cJSON_GetObjectItem(final, "l");
			u8 l_val = (u8)cJSON_GetNumberValue(l);

			cJSON *pc = cJSON_GetObjectItem(final, "pc");
			u16 pc_val = (u16)cJSON_GetNumberValue(pc);

			cJSON *sp = cJSON_GetObjectItem(final, "sp");
			u16 sp_val = (u16)cJSON_GetNumberValue(sp);

			printf("a: %d\nb: %d\nc: %d\nd: %d\ne: %d\nf: %d\nh: %d\nl: %d\npc: %d\nsp: %d\n",
				   a_val, b_val, c_val, d_val, e_val, f_val, h_val, l_val, pc_val, sp_val);

			cJSON *ram = cJSON_GetObjectItem(final, "ram");
			int ram_size = cJSON_GetArraySize(ram);

			for (int j = 0; j < ram_size; ++j) {
				cJSON *ram_entry = cJSON_GetArrayItem(ram, j);

				cJSON *ram_address = cJSON_GetArrayItem(ram_entry, 0);
				u16 ram_address_num = (u16)cJSON_GetNumberValue(ram_address);

				cJSON *ram_value = cJSON_GetArrayItem(ram_entry, 1);
				u16 ram_value_num = (u16)cJSON_GetNumberValue(ram_value);

				printf("ram:\naddress:%d\nvalue:%d\n", ram_address_num, ram_value_num);
			}

			printf("\n=============================================\n\n");
		}
	}
}
