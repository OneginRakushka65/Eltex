#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_PACKAGE_SIZE 100
#define MAX_MTEXT 256
#define SERVER_ID 10
#define MAX_CLIENT_SIZE 10

typedef struct {
    long mtype;
    int sender_id;
    char msg_text[MAX_PACKAGE_SIZE];
} msg_buffer;