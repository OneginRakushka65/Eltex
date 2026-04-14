#include "main.h"

int clients[MAX_CLIENT_SIZE];
int client_count;
int que;

int main() {
    msg_buffer message;
    key_t key;

    if ((key = ftok("server_file", 1)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }

    if ((que = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget failed:: ");
        exit(1);
    }

    while(1) {
        if (msgrcv(que, &message, sizeof(message) - sizeof(long), SERVER_ID, 0) == -1) {
            perror("msgrcv");
            continue;
        }

        printf("Получено от клиента %d: %s\n", message.sender_id, message.msg_text);

        int found = 0;
        for (int i = 0; i < MAX_CLIENT_SIZE; i++) {
            if (clients[i] == message.sender_id) {
                found = 1;
                break;
            }
        }

        if (!found && client_count < MAX_CLIENT_SIZE) {
            clients[client_count++] = message.sender_id;
            printf("Новый клиент зарегистрирован: %d\n", message.sender_id);
        }

        if (strcmp(message.msg_text, "shutdown") == 0) {
            for (int i = 0; i < client_count; i++) {
                if (clients[i] == message.sender_id) {
                    for (int j = i; j < client_count - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    clients[client_count - 1] = 0;
                    client_count--;
                    printf("Клиент %d покидает чат.\n", message.sender_id);
                    break;
                }
            }
            continue;
        }

        for (int i = 0; i < MAX_CLIENT_SIZE; i++) {
            if (clients[i] != 0 && clients[i] != message.sender_id) {
                message.mtype = clients[i];
                msgsnd(que, &message, sizeof(message) - sizeof(long), 0);
            }
        }
     }
}