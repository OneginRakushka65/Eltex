#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_SIZE 100
#define MAX_ARGS_SIZE 20

enum Command {CMD_EXIT, CMD_SUM, CMD_3_01, CMD_UNKNOWN};

enum Command parse_command(const char* command) {
	if (strcmp(command, "exit") == 0) return CMD_EXIT;
	if (strcmp(command, "sum") == 0) return CMD_SUM;
  if (strcmp(command, "3_01") == 0) return CMD_3_01;
	return CMD_UNKNOWN;
}

int command_handler() {
	printf("Module 3 Task 2 for Eltex now working\n");
	printf("Features: sum [a] [b], 3_01 [a] ..., exit\n");

	while (1) {
		printf(">> ");

		char command[MAX_COMMAND_SIZE];
		char* token;

		fgets(command, sizeof(command), stdin);
		command[strcspn(command, "\n")] = 0;

		token = strtok(command, " ");
		if (!token) continue;

		char* arg[MAX_ARGS_SIZE];
		int i = 0;

		arg[i++] = token;

		token = strtok(NULL, " ");
		while (i < (MAX_ARGS_SIZE - 1) && token != NULL) {
			arg[i++] = token;
			token = strtok(NULL, " ");
		}
		arg[i] = NULL;
 
		switch (parse_command(arg[0])) {
			case CMD_EXIT:
				printf("Goodbye! Thanks for using command interpreter!\n");
				exit(0);
			case CMD_SUM: {
				pid_t pid = fork();
				if (pid < 0) {
					perror("Fork failed");
				}
				else if (pid == 0) {
					if (execvp("./sum", arg) == 1) {
						perror("Exec failed");
						exit(1);
					}
				}
				else {
					wait(NULL);
				}
				break;
			}
			case CMD_3_01: {
				pid_t pid = fork();
				if (pid < 0) {
					perror("Fork failed");
				}
				else if (pid == 0) {
					if (execvp("./3_01", arg) == 1) {
						perror("Exec failed");
						exit(1);
					}
				}
				else {
				wait(NULL);
				}
				break;
			}
			case CMD_UNKNOWN: {
                pid_t pid = fork();
                if (pid < 0) {
                    perror("Fork failed");
                } 
                else if (pid == 0) {
                    if (execvp(arg[0], arg) == -1) {
                        fprintf(stderr, "Error: Command '%s' not found\n", arg[0]);
                        exit(1);
                    }
                } 
                else {
                    wait(NULL);
                }
                break;
            }
		}
	}
}

int main() {
	int out = command_handler();
}