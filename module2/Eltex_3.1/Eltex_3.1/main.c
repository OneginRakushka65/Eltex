#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef unsigned short mode_t;

void print_bits(mode_t mode) {
	for (int i = 8; i >= 0; i--) {
		printf("%d", (mode >> i) & 1);
	}
	printf("\n");
}

void print_symbols(mode_t mode) {
	for (int i = 8; i >= 0; i--) {
		if ((mode >> i) & 1) {
			switch (i % 3) {
			case 0:
				printf("x");
				break;
			case 1:
				printf("w");
				break;
			case 2:
				printf("r");
				break;
			}
		}
		else printf("-");
	}
	printf("\n");
}

void modify(mode_t* mode, const char *cmd) {
	mode_t target = 0;
	int next = 0;
	while (next == 0 && *cmd != '\0') {
		switch (*(cmd++)) {
		case 'u':
			target |= 0700;
			break;
		case 'g':
			target |= 0070;
			break;
		case 'o':
			target |= 0007;
			break;
		case 'a':
			target |= 0777;
			break;
		case '+':
			next = 1;
			break;
		case '-':
			next = 2;
			break;
		case '=':
			next = 3;
			break;
		}
	}
	mode_t priv = 0;
	while (*cmd != '\0' && *cmd != ' ') {
		switch (*(cmd++)) {
		case 'r':
			priv |= 0444;
			break;
		case 'w':
			priv |= 0222;
			break;
		case 'x':
			priv |= 0111;
			break;
		}
	}

	switch (next) {
	case 1:
		*mode |= (target & priv);
		break;
	case 2:
		*mode &= ~(target & priv);
		break;
	case 3:
		*mode = (target & priv);
		break;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: %s [number OR filename]\n", argv[0]);
		return 1;
	}

	mode_t mode = 0;
	struct stat st;

	if (stat(argv[1], &st) == 0) {
		mode = st.st_mode & 0777;
		printf("Reading from file: %s\n", argv[1]);
	}
	else {
		mode = (mode_t)strtol(argv[1], NULL, 8);
		printf("Manual input (octal): %s\n", argv[1]);
	}

	printf("Octal: %03o\n", mode);
	print_bits(mode);
	print_symbols(mode);
	printf("\n");

	if (argc >= 3) {
		printf("Applying modifier: %s\n", argv[2]);

		modify(&mode, argv[2]);

		printf("Result octal: %03o\n", mode);
		print_bits(mode);
		print_symbols(mode);
		printf("\n");
	}

	return 0;
}