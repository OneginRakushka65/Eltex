#include "main.h"

void dostuff(int sock) {
    while(1) {
        char buffer[MAX_SIZE];
        int n;
        double a, b, result;
        char op;

        char *msg0 = "Выберите операцию (+, -, *, /) или 'q' для выхода: ";
        write(sock, msg0, strlen(msg0));

        memset(buffer, 0, MAX_SIZE);
        n = read(sock, buffer, MAX_SIZE - 1);
        if (n <= 0) {
            printf("Клиент завершил соединение.\n");
            return; 
        }

        op = buffer[0];

        if (op == 'q') return;

        if (op == 'f') {
            write(sock, "READY", 5);
    
            FILE *f = fopen("received_data.txt", "wb"); // Открываем файл для записи
            if (f == NULL) {
                perror("Ошибка создания файла");
                break;
            }

            memset(buffer, 0, MAX_SIZE);
            while ((n = read(sock, buffer, MAX_SIZE)) > 0) {
                if (strstr(buffer, "DONE") != NULL) break;
                fwrite(buffer, 1, n, f);
                memset(buffer, 0, MAX_SIZE);
            }
            fclose(f);

            char *last = "Файл получен\n";

            write(sock, last, strlen(last));
            continue;
        }

        char *msg1 = "Введите первое число: ";
        write(sock, msg1, strlen(msg1));
        memset(buffer, 0, MAX_SIZE);
        read(sock, buffer, MAX_SIZE);
        a = atof(buffer);

        char *msg2 = "Введите второе число: ";
        write(sock, msg2, strlen(msg2));
        memset(buffer, 0, MAX_SIZE);
        read(sock, buffer, MAX_SIZE);
        b = atof(buffer);

        switch(op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/': 
                if (b == 0) {
                    char *err = "Ошибка: деление на ноль!\n";
                    write(sock, err, strlen(err));
                    continue;
                }
                result = a / b; 
                break;
            default:
                write(sock, "Неверная операция\n", 18);
                return;
        }

        snprintf(buffer, MAX_SIZE, "Результат: %.2f\n", result);
        write(sock, buffer, strlen(buffer));
    }
}

int main() {
    int tcp_socket;
    struct sockaddr_in server_addr, client_addr;

    if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed:: ");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    memset(&client_addr, 0, sizeof(client_addr));

    socklen_t client_size = sizeof(client_addr);

    if (bind(tcp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed:: ");
        exit(1);
    }

    printf("Сервер запущен и ждёт\n");

    listen(tcp_socket, 5);

    while(1) {
        int newsock = accept(tcp_socket, (struct sockaddr*)&client_addr, &client_size);

        if (newsock < 0) {
            perror("accept failed");
            continue;
        }
        printf("Клиент подключился!\n");

        pid_t pid = fork();

        if (pid == 0) {
            close(tcp_socket);
            dostuff(newsock);
            exit(0);
        } else if (pid > 0) {
            close(newsock);
        }
    }
}