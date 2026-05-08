#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_COMMAND_SIZE 100
#define MAX_ARGS_SIZE 20
#define _POSIX_C_SOURCE 200809L

enum Command {CMD_EXIT, CMD_SUM, CMD_3_01, CMD_UNKNOWN};

typedef struct CommandNode {
    char* args[MAX_ARGS_SIZE];
    char* input;
    char* output;
    struct CommandNode* next;
} CommandNode;

enum Command parse_command(const char* command) {
	if (strcmp(command, "exit") == 0) return CMD_EXIT;
	if (strcmp(command, "sum") == 0) return CMD_SUM;
  if (strcmp(command, "3_01") == 0) return CMD_3_01;
	return CMD_UNKNOWN;
}

void free_pipeline(CommandNode* head) {
    CommandNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

CommandNode* execute_pipeline(char* line) {
    CommandNode* head = NULL;
    CommandNode* last = NULL;

    char* saveptr1;
    char* cmd_str = strtok_r(line, "|", &saveptr1);

    while (cmd_str != NULL) {
        CommandNode* node = calloc(1, sizeof(CommandNode));

        int arg_idx = 0;
        char* saveptr2;
        while (*cmd_str == ' ') cmd_str++;
        char* token = strtok_r(cmd_str, " ", &saveptr2);

        while (token != NULL) {
            if (strcmp(token, ">") == 0) {
                node->output = strtok_r(NULL, " ", &saveptr2);
            } else if (strcmp(token, "<") == 0) {
                node->input = strtok_r(NULL, " ", &saveptr2);
            } else {
                node->args[arg_idx++] = token;
            }
            token = strtok_r(NULL, " ", &saveptr2);
        }
        node->args[arg_idx] = NULL;
        
        if (!head) head = node;
        else last->next = node;
        last = node;

        cmd_str = strtok_r(NULL, "|", &saveptr1);
    }
    return head;
}

int command_handler() {
    printf("Module 3 Task 2 for Eltex now working\n");

    while (1) {
        printf(">> ");
        char line[MAX_COMMAND_SIZE];
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) continue;

        CommandNode* head = execute_pipeline(line); 
        CommandNode* current = head;

        int in_fd = STDIN_FILENO;

        while (current != NULL) {
            if (parse_command(current->args[0]) == CMD_EXIT) {
                printf("Goodbye!\n");
                exit(0);
            }

            int pipefds[2];
            if (current->next) {
                pipe(pipefds);
            }

            pid_t pid = fork();
            if (pid == 0) {
                if (in_fd != STDIN_FILENO) {
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                }
                if (current->next) {
                    close(pipefds[0]);
                    dup2(pipefds[1], STDOUT_FILENO);
                    close(pipefds[1]);
                }

                if (current->input) {
                    int fd = open(current->input, O_RDONLY);
                    if (fd != -1) { dup2(fd, 0); close(fd); }
                    else { perror("Input file error"); exit(1); }
                }
                if (current->output) {
                    int fd = open(current->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd != -1) { dup2(fd, 1); close(fd); }
                    else { perror("Output file error"); exit(1); }
                }

                char* path = current->args[0];
                enum Command type = parse_command(path);
                if (type == CMD_SUM) path = "./sum";
                else if (type == CMD_3_01) path = "./3_01";

                execvp(path, current->args);
                fprintf(stderr, "Command '%s' not found\n", current->args[0]);
                exit(1);
            } 
            else if (pid > 0) {
                if (in_fd != STDIN_FILENO) close(in_fd);
                if (current->next) {
                    close(pipefds[1]);
                    in_fd = pipefds[0];
                }
            }
            
            current = current->next;
        }
        while (wait(NULL) > 0);
        
        free_pipeline(head);
        if (in_fd != STDIN_FILENO) close(in_fd);
    }
    return 0;
}

int main() {
	int out = command_handler();
}