#include "main.h"

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        printf("Использование: %s <имя_файла>\n", argv[0]);
        exit(1);
    }
    
    const char *filename = argv[1];
    key_t key;
    int semid;

    if ((key = ftok(filename, 1)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }

    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget failed:: ");
        exit(1);
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl failed:: ");
        exit(1);
    }

    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};

    srand(time(NULL));
    int r;

    FILE* f_clear = fopen(filename, "w");
    if (f_clear == NULL) {
        perror("Ошибка при очистке файла");
        exit(1);
    }
    fclose(f_clear);

    while(1) {
        r = rand();

        semop(semid, &lock, 1);

        FILE* file = fopen(filename, "a");
        
        if (file) {
            int count = 1 + r % MAX_NUMS;
            for (int i = 1; i <= count; i++) {
                r = rand();
                fprintf(file, "%d ", rand() % 100);
            } 
            fprintf(file, "\n");
            fclose(file);

            printf("Производитель записал строку в %s\n", filename);
        }

        semop(semid, &unlock, 1);
        sleep(1);
    }
    
    return 0;
}