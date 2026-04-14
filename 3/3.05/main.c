#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define BUFFER_MAX_SIZE 50

int sigint_count = 0;
int fd;

void signal_handle(int signal) {
    if (signal == SIGINT) {
        sigint_count++;
        if (sigint_count >= 3) {
            write(fd, "Third SIGINT: Exit\n", 19);
            close(fd);
            exit(0);
        }
        write(fd, "SIGINT\n", 7);
    } else if (signal == SIGQUIT) {
        write(fd, "SIGQUIT\n", 8);
    }
}

int main() {
    fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("File error");
        return 1;
    }

    signal(SIGINT, signal_handle);
    signal(SIGQUIT, signal_handle);

    char buffer[BUFFER_MAX_SIZE];
    int i = 0;

    while(1) {
        int length = snprintf(buffer, sizeof(buffer), "Score: %d\n", i++);

        write(fd, buffer, length);

        sleep(1);
    }
}