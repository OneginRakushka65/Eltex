#include "main.h"

struct ClientContext clients[MAX_CLIENT];

int handle_client_data(int sock, int client_index) {
    char buffer[MAX_SIZE];
    memset(buffer, 0, MAX_SIZE);

    int n = read(sock, buffer, MAX_SIZE - 1);
    if (n <= 0) {
        // Если принимали файл и клиент отвалился, закрываем файл, чтобы не было утечек
        if (clients[client_index].state == STATE_FILE_RX && clients[client_index].file != NULL) {
            fclose(clients[client_index].file);
        }
        return -1; // Сигнал для main, что клиента надо отключить
    }

    switch(clients[client_index].state) {
        
        case STATE_WAIT_OP:
            clients[client_index].op = buffer[0]; // Запоминаем операцию
            char op = clients[client_index].op;

            if (op == 'q') return -1; // Отключаем клиента

            if (op == 'f') {
                write(sock, "READY\n", 6);
                
                // Создаем уникальное имя файла для каждого клиента (опционально)
                char filename[32];
                snprintf(filename, sizeof(filename), "client_%d_data.txt", client_index);
                clients[client_index].file = fopen(filename, "wb");
                
                clients[client_index].state = STATE_FILE_RX; // Переключаем состояние
            } else {
                char *msg1 = "Введите первое число: ";
                write(sock, msg1, strlen(msg1));
                clients[client_index].state = STATE_WAIT_A; // Переключаем состояние
            }
            break;

        case STATE_WAIT_A:
            clients[client_index].a = atof(buffer); // Запоминаем первое число
            
            char *msg2 = "Введите второе число: ";
            write(sock, msg2, strlen(msg2));
            
            clients[client_index].state = STATE_WAIT_B; // Переключаем состояние
            break;

        case STATE_WAIT_B:
            {
                double a = clients[client_index].a; // Достаем из памяти
                double b = atof(buffer);            // Берем из буфера
                double result = 0;
                char op = clients[client_index].op; // Достаем операцию

                // Считаем
                if (op == '+') result = a + b;
                else if (op == '-') result = a - b;
                else if (op == '*') result = a * b;
                else if (op == '/') {
                    if (b == 0) {
                        write(sock, "Ошибка: деление на ноль!\n", 45);
                        // Сбрасываем состояние обратно на начало
                        clients[client_index].state = STATE_WAIT_OP; 
                        return 0;
                    }
                    result = a / b;
                }

                snprintf(buffer, MAX_SIZE, "Результат: %.2f\nВыберите операцию: ", result);
                write(sock, buffer, strlen(buffer));
                
                clients[client_index].state = STATE_WAIT_OP; // Возвращаемся в начало
            }
            break;

        case STATE_FILE_RX:
            // Важно: никаких while внутри! Пишем кусок, который пришел, и уходим.
            if (strstr(buffer, "DONE") != NULL) {
                fclose(clients[client_index].file);
                clients[client_index].file = NULL; // Обнуляем указатель
                
                char *msg = "Файл получен\nВыберите операцию: ";
                write(sock, msg, strlen(msg));
                
                clients[client_index].state = STATE_WAIT_OP; // Возвращаемся в начало
            } else {
                fwrite(buffer, 1, n, clients[client_index].file);
            }
            break;
    }

    return 0;
}

int main() {
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed:: ");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen failed");
        exit(1);
    }

    printf("Сервер готов к работе на порту 5000\n");

    struct pollfd fds[MAX_CLIENT];
    for (int i = 0; i < MAX_CLIENT; i++) {
        fds[i].fd = -1;
    }
    
    int fdsc = 1;
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    while(1) {
        int poll_count = poll(fds, fdsc, -1);
        if (poll_count < 0) {
            perror("poll error");
            break;
        }

        if (fds[0].revents & POLLIN) {
            int newsock = accept(server_socket, NULL, NULL);
            for (int i = 1; i < MAX_CLIENT; i++) {
                if (fds[i].fd == -1) {
                    fds[i].fd = newsock;

                    clients[i].state = STATE_WAIT_OP;
                    clients[i].file = NULL;
                    write(newsock, "Выберите операцию: ", 36);

                    fds[i].events = POLLIN;
                    if (i >= fdsc) {
                        fdsc = i + 1;
                    }
                    printf("Клиент занял ячейку %d\n", i);
                    break;
                }
            }
        }

        for (int i = 1; i < fdsc; i++) {
            if (fds[i].fd != -1 && fds[i].revents & POLLIN) {
                int status = handle_client_data(fds[i].fd, i);

                if (status == -1) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    
                    if (i == fdsc - 1) {
                        while (fdsc > 1 && fds[fdsc - 1].fd == -1) {
                            fdsc--;
                        }
                    }
                }
            }
        }
    }
}