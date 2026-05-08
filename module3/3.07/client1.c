#include "main.h"

int main() {
    key_t s_key;
    key_t r_key;

    int s_que; //отправлять
    int r_que; //получать

    msg_buffer message;

    if ((s_key = ftok("msg_file", 1)) == -1 || (r_key = ftok("msg_file", 2)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }

    if ((s_que = msgget(s_key, 0666 | IPC_CREAT)) == -1 || (r_que = msgget(r_key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget failed:: ");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {
        while(1) {
            if (msgrcv(r_que, &message, sizeof(msg_buffer) - sizeof(long), -10, 0) != -1) {
                if (message.mtype == 1) {
                    printf("\nСобеседник завершил чат.\n");
                    kill(getppid(), SIGKILL);
                    exit(0);
                }

                printf("Собеседник: %s\n", message.text);
            }
        }
    } else if (pid > 0) {
        while(1) {
            fgets(message.text, MAX_MESSAGE_LENGTH, stdin);
            message.text[strcspn(message.text, "\n")] = 0;

            if (strcmp(message.text, "exit") == 0) {
                message.mtype = 1;
                msgsnd(s_que, &message, sizeof(msg_buffer) - sizeof(long), 0);
                kill(pid, SIGKILL);
                exit(0);
            }

            message.mtype = 10;

            msgsnd(s_que, &message, sizeof(msg_buffer) - sizeof(long), 0);
        }
    }
}