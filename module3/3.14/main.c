#include "main.h"

int main() {
    int my_socket;
    char buffer[MAX_SIZE];

    if ((my_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket failed:: ");
        exit(1);
    }

    while(1) {
        ssize_t data_size;
        if ((data_size = recvfrom(my_socket, buffer, MAX_SIZE, 0, NULL, NULL)) < 0) {
            perror("recvfrom failed:: ");
            break;
        }

        struct iphdr* ip = (struct iphdr*)buffer;
        struct udphdr* udp = (struct udphdr*)(buffer + ip->ihl * 4);

        unsigned short port_from = ntohs(udp->source);
        unsigned short port_to = ntohs(udp->dest);

        if (port_to == SERVER_PORT) {
            char* data = buffer + ip->ihl * 4 + sizeof(*udp);

            int data_len = data_size - ip->ihl * 4 - sizeof(*udp);

            printf("[Порт %u]: ", port_from);
            for (int i = 0; i < data_len; i++) {
                printf("%c", data[i]);
            }
            fflush(stdout);
        }
    }
}