#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define MAX_CLIENT 10
#define MAX_SIZE 1024

#define STATE_WAIT_OP 0
#define STATE_WAIT_A  1
#define STATE_WAIT_B  2
#define STATE_FILE_RX 3

struct ClientContext {
    int state;
    char op;
    double a;
    FILE *file;
};