#include "main.h"

int main(const int argc, const char* argv[]) {
    if (argc < 3) {
        printf("Использование: %s <мой_порт> <порт_друга>\n", argv[0]);
        exit(1);
    }

    int my_port = atoi(argv[1]);
    int fr_port = atoi(argv[2]);

    int udp_socket;
    struct sockaddr_in my_addr, fr_addr;

    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket failde:: ");
        exit(1);
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(my_port);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    memset(&fr_addr, 0, sizeof(fr_addr));
    fr_addr.sin_family = AF_INET;
    fr_addr.sin_port = htons(fr_port);
    inet_pton(AF_INET, "127.0.0.1", &fr_addr.sin_addr);

    if (bind(udp_socket, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1) {
        perror("bind failed:: ");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {
        char message[MAX_SIZE];
        while(1) {
            if (fgets(message, MAX_SIZE, stdin)) {
                sendto(udp_socket, message, strlen(message), 0, (struct sockaddr*)&fr_addr, sizeof(fr_addr));
            }
        }
    } else if (pid > 0) {
        char message[MAX_SIZE];
        while(1) {
            int res = recvfrom(udp_socket, message, MAX_SIZE - 1, 0, NULL, NULL);
            if (res > 0) {
                message[res] = '\0';
                printf("\n[Сообщение]: %s", message);
                fflush(stdout);
            }
        }
    }
}