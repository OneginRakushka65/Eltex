#define _CRT_SECURE_NO_WARNINGS
#include "main.h"

int main() {
	char command[10];
	while (1) {
		fgets(command, sizeof(command), stdin);
		command[strcspn(command, "\n")] = 0;
		
		char* token = strtok(command, " ");
		double a = strtod(token, NULL);

		token = strtok(NULL, " ");
		char sign = *token;

		token = strtok(NULL, " ");
		double b = strtod(token, NULL);

		for (int i = 0; i < (sizeof(pool) / sizeof(pool[0])); i++) {
			if (pool[i].sign == sign) {
				double result = pool[i].function(a, b);
				printf("%.2lf\n", result);
				break;
			}
		}
	}
}