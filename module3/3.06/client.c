#include "main.h"

int que;

int main(const int argc, char* argv[]) {
    key_t key;
    msg_buffer message;
    int msg_id;

    if (argc != 2 || (msg_id = strtod(argv[1], NULL)) % 10 != 0) {
        perror("args failed:: ");
        exit(1);
    }

    if ((key = ftok("server_file", 1)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }

    if ((que = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget failed:: ");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {
        while (1) {
            char text[MAX_PACKAGE_SIZE];

            fgets(message.msg_text, MAX_PACKAGE_SIZE, stdin);
            message.msg_text[strcspn(message.msg_text, "\n")] = 0;

            message.mtype = 10;
            message.sender_id = msg_id;

            msgsnd(que, &message, sizeof(msg_buffer) - sizeof(long), 0);
        } 
    }  
    else if (pid > 0) {
        while (1) {
            if (msgrcv(que, &message, sizeof(msg_buffer) - sizeof(long), msg_id, 0) != -1) {
                printf("Чат: %s\n", message.msg_text);
            }
        }
    }
}