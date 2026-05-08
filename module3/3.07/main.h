#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>

#define MAX_MESSAGE_LENGTH 256

typedef struct {
    long mtype;
    char text[MAX_MESSAGE_LENGTH];
} msg_buffer;