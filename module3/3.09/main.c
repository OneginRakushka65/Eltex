#include "main.h"

sem_t *sem = NULL;

void handler(int sig) {
    if (sem != NULL && sem != SEM_FAILED) {
        sem_close(sem);
        sem_unlink(SEM_NAME);
    }
    printf("\nСемафор успешно удален. Выход...\n");
    exit(0);
}

int main(const int argc, const char* argv[]) {
    signal(SIGINT, handler);

    if (argc < 2) {
        printf("Использование: %s <имя_файла>\n", argv[0]);
        exit(1);
    }
    
    const char *filename = argv[1];

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    srand(time(NULL));

    FILE* f_clear = fopen(filename, "w");
    if (f_clear == NULL) {
        perror("Ошибка при очистке файла");
        sem_close(sem);
        sem_unlink(SEM_NAME);
        exit(1);
    }
    fclose(f_clear);

    pid_t pid = fork();

    if (pid == 0) {
        char line[1024];
        while(1) {
            sem_wait(sem);

            FILE* file = fopen(filename, "r");
            if (file) {
                int found_any = 0;
                while(fgets(line, sizeof(line), file)) {
                    int min = 101, max = -1;
                    int val, offset;
                    char *ptr = line;
                    int found_in_line = 0;

                    while(sscanf(ptr, "%d%n", &val, &offset) == 1) {
                        if (val < min) min = val;
                        if (val > max) max = val;
                        ptr += offset;
                        found_in_line = 1;
                    }

                    if (found_in_line) {
                        printf("[Потребитель] Анализ: Min = %d, Max = %d\n", min, max);
                        found_any = 1;
                    }
                }
                fclose(file);

                if (found_any) {
                    FILE* f_wipe = fopen(filename, "w");
                    if (f_wipe) fclose(f_wipe);
                }
            }

            sem_post(sem);
            sleep(2);
        }
    } else if (pid > 0) {
        while(1) {
            // Блокировка
            sem_wait(sem);

            FILE* file = fopen(filename, "a");
            if (file) {
                int count = 1 + rand() % MAX_NUMS;
                for (int i = 0; i < count; i++) {
                    fprintf(file, "%d ", rand() % 100);
                } 
                fprintf(file, "\n");
                fclose(file);
                printf("[Производитель] Записал %d чисел в файл\n", count);
            }

            sem_post(sem);
            sleep(1);
        }
    }

    return 0;
}