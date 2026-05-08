#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NOT_ENOUGH_ARGUMENTS 1

int is_digit(char* sign) {
	if (sign == NULL || strlen(sign) == 0) return 0;

	int count = 0;
	int has_digits = 0;

	for (int i = 0; sign[i] != '\0'; i++) {
		if (sign[i] == '\n') continue;
		if (sign[i] == '.') {
			count++;
			if (count > 1 || i == 0 || sign[i + 1] == '\0') return 0;
			continue;
		}
		if (!isdigit(sign[i]) || count > 1) return 0;
		has_digits = 1;
	}

	return has_digits;
}

int main(int argc, char* argv[]) {
	if (argc <= 1) return NOT_ENOUGH_ARGUMENTS;

	pid_t pid = fork();

	if (pid < 0) {
		perror("Fork failed");
		return 1;
	}

	if (pid == 0) {
		for (int i = 1; i < argc; i += 2) {
			if (is_digit(argv[i])) printf("[KID]It's number: %s %f\n", argv[i], strtod(argv[i], NULL) * 2);
			else printf("[KID]It's word: %s\n", argv[i]);
		}
		exit(0);
	}
	else {
		for (int i = 2; i < argc; i += 2) {
			if (is_digit(argv[i])) printf("[PARENT]It's number: %s %f\n", argv[i], strtod(argv[i], NULL) * 2);
			else printf("[PARENT]It's word: %s\n", argv[i]);
		}
		wait(NULL);
	}
}