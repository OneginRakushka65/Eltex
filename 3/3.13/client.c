#include "main.h"

int main(int argc, char *argv[]) {
    int my_sock;
    struct sockaddr_in serv_addr;

    if ((my_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(my_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Подключено к серверу!\n");

    char buffer[MAX_SIZE];
    while(1) {
        memset(buffer, 0, MAX_SIZE);
        int n = read(my_sock, buffer, MAX_SIZE - 1);
        if (n <= 0) { 
            printf("Соединение разорвано сервером.\n");
            break; 
        }
        printf("%s", buffer);

        memset(buffer, 0, MAX_SIZE);
        fgets(buffer, MAX_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (buffer[0] == 'q') {
            write(my_sock, "q", 1);
            break;
        }

        if (buffer[0] == 'f') {
            write(my_sock, "f", 1);

            memset(buffer, 0, MAX_SIZE);
            read(my_sock, buffer, MAX_SIZE); 
            if (strncmp(buffer, "READY", 5) != 0) continue;
            printf("Сервер ответил: %s\n", buffer);

            FILE *f = fopen("test.txt", "rb");
            if (f == NULL) {
                printf("Ошибка: создайте файл test.txt перед отправкой!\n");
                write(my_sock, "DONE", 4); 
                continue;
            }

            printf("Отправка файла...\n");
            while (!feof(f)) {
                int bytes_read = fread(buffer, 1, MAX_SIZE, f);
                if (bytes_read > 0) {
                    write(my_sock, buffer, bytes_read);
                }
            }
            
            fclose(f);
            sleep(1); 
            write(my_sock, "DONE", 4);
            printf("Файл отправлен.\n");

            memset(buffer, 0, MAX_SIZE);
            read(my_sock, buffer, MAX_SIZE); 
            printf("Сервер: %s", buffer);

            continue; 
        }

        write(my_sock, buffer, strlen(buffer));
    }
}