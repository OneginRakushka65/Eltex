#include "main.h"

int total_processed = 0;
shared_data* data = NULL;
sem_t *sem = NULL;

void sigint_handler(int sig) {
    printf("\nПолучен SIGINT (Ctrl+C). Обработано наборов: %d\n", total_processed);
    munmap(data, sizeof(shared_data));
    shm_unlink(SHM_NAME);

    sem_close(sem);
    sem_unlink(SEM_NAME);
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);

    int fd;
    if ((fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open failed:: ");
        exit(1);
    }
    ftruncate(fd, sizeof(shared_data)); 

    if ((data = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("mmap failed:: ");
        exit(1);
    }

    if ((sem = sem_open(SEM_NAME, O_CREAT, 0666, 1)) == SEM_FAILED) {
        perror("sem_open failed:: ");
        exit(1);
    }

    data->ready = 0;
    data->count = 0;

    pid_t pid = fork();

    srand(time(NULL));
    int r;

    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        while(1) {

            sem_wait(sem);

            if (data->ready == 1) {
                int min = MAX_NUMBER + 1, max = -1;
                if (data->count > 0) {
                    for (int i = 0; i < data->count; i++) {
                        if (data->numbers[i] < min) min = data->numbers[i];
                        if (data->numbers[i] > max) max = data->numbers[i];
                    }
                }

                data->min = min;
                data->max = max;

                data->ready = 2;
            }

            sem_post(sem);
            usleep(50000);
        }
    } else if (pid > 0) {
        while(1) {

            sem_wait(sem);

            if (data->ready == 2) {
                printf("Строка проанализирована: Min = %d, Max = %d\n", data->min, data->max);
                total_processed++;
                data->ready = 0;   
            }

            if (data->ready == 0) {
                data->count = 0;

                int count = 1 + rand() % 10;
                for (int i = 0; i < count; i++) {
                    r = rand() % MAX_NUMBER;
                    data->numbers[i] = r;
                    data->count++;
                    data->max = -1;
                    data->min = -1;
                }

                data->ready = 1;
            }

            sem_post(sem);
            sleep(1);
        }
    }
}