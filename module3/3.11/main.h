#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define MAX_NUMBER 100

#define SHM_NAME "/my_shared_memory"
#define SEM_NAME "/my_semaphore"

typedef struct {
    int numbers[100];
    int count;
    int max;
    int min;
    int ready;
} shared_data;