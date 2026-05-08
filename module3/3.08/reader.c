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

    char line[1024];

    while(1) {
        semop(semid, &lock, 1);

        FILE* file = fopen(filename, "r");
        
        if (file) {
            while(fgets(line, sizeof(line), file)) {
                int min = 101, max = -1;
                int val, offset;
                char *ptr = line;
                int found = 0;

                while(sscanf(ptr, "%d%n", &val, &offset) == 1) {
                    if (val < min) min = val;
                    if (val > max) max = val;

                    ptr += offset;
                    found = 1;
                }

                if (found) {
                    printf("Строка проанализирована: Min = %d, Max = %d\n", min, max);
                }
            }
            fclose(file);

            FILE* f_wipe = fopen(filename, "w");
            if (f_wipe) fclose(f_wipe);
        }

        semop(semid, &unlock, 1);
        sleep(2);
    }
    
    return 0;
}