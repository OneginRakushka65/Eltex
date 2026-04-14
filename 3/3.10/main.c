#include "main.h"

int total_processed = 0;
int shmid;
int semid;

void sigint_handler(int sig) {
    printf("\nПолучен SIGINT (Ctrl+C). Обработано наборов: %d\n", total_processed);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);

    key_t shmkey;
    shared_data* data;

    if ((shmkey = ftok("shm_file", 1)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }

    if ((shmid = shmget(shmkey, sizeof(shared_data), 0666 | IPC_CREAT)) == -1) {
        perror("shmget failed:: ");
        exit(1);
    }

    if ((data = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("shmat failed:: ");
        exit(1);
    }

    key_t semkey;

    if ((semkey = ftok("shm_file", 1)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }

    if ((semid = semget(semkey, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget failed:: ");
        exit(1);
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl failed:: ");
        exit(1);
    }

    data->ready = 0;
    data->count = 0;

    pid_t pid = fork();

    srand(time(NULL));
    int r;

    struct sembuf lock = {0, -1, SEM_UNDO};
    struct sembuf unlock = {0, 1, SEM_UNDO};

    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        while(1) {

            semop(semid, &lock, 1);

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

            semop(semid, &unlock, 1);
            usleep(50000);
        }
    } else if (pid > 0) {
        while(1) {

            semop(semid, &lock, 1);

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

            semop(semid, &unlock, 1);
            sleep(1);
        }
    }
}