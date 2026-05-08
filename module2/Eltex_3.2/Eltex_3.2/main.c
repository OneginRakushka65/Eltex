#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

uint32_t char_to_int(const char* string) {
	uint32_t t, e, m, p;
	sscanf(string, "%u.%u.%u.%u", &t, &e, &m, &p);
	return t << 24 | e << 16 | m << 8 | p;
}

void int_to_char(const uint32_t ip, char* buffer) {
	sprintf(buffer, "%u.%u.%u.%u", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
}

uint32_t mask_parser(const char* string) {
	if (string[0] == '/') {
		return 0xFFFFFFFF << (32 - atoi(string + 1));
	}
	else return char_to_int(string);
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("Feature: ./main [gate_ip] [mask] [package_count]");
		return 1;
	}

	uint32_t gate_ip = char_to_int(argv[1]);
	uint32_t mask = mask_parser(argv[2]);
	int package_count = atoi(argv[3]);
	uint32_t network = gate_ip & mask;

	srand(time(NULL));

	int local_count = 0;      // пакеты в своей сети
	int external_count = 0;   // пакеты в других сетях

	for (int i = 0; i < package_count; i++) {
		uint32_t random_ip = ((uint32_t)(rand() % 256) << 24) |
							 ((uint32_t)(rand() % 256) << 16) |
							 ((uint32_t)(rand() % 256) << 8) |
							 (rand() % 256);

		if ((random_ip & mask) == network) local_count++;
		else external_count++;
	}

	printf("Packages: %d\n", package_count);
	printf("in own network: %d (%.2f%%)\n", local_count, 100.0 * local_count / package_count);
	printf("in diff network: %d (%.2f%%)\n", external_count, 100.0 * external_count / package_count);
}