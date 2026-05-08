#define _CRT_SECURE_NO_WARNINGS
#include "main.h"

void load_plugins(const char* folder) {
    char search_path[MAX_PATH];
    sprintf(search_path, "%s\\*.dll", folder);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(search_path, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Plugin folder not found or empty.\n");
        return;
    }

    do {
        char dll_path[MAX_PATH];
        sprintf(dll_path, "%s\\%s", folder, findData.cFileName);

        HMODULE hLib = LoadLibraryA(dll_path);
        if (hLib) {
            sign_func get_s = (sign_func)GetProcAddress(hLib, "get_sign");
            calc_func calc = (calc_func)GetProcAddress(hLib, "calculate");

            if (get_s && calc) {
                pool[op_count].sign = get_s();
                pool[op_count].function = calc;
                pool[op_count].hLib = hLib;
                printf("Loaded plugin: [%s] for operation '%c'\n", findData.cFileName, pool[op_count].sign);
                op_count++;
            }
            else {
                printf("Error: DLL %s is missing required functions.\n", findData.cFileName);
                FreeLibrary(hLib);
            }
        }
    } while (FindNextFileA(hFind, &findData) && op_count < MAX_PLUGINS);

    FindClose(hFind);
}

int main() {
    load_plugins("plugins");

    if (op_count == 0) {
        printf("No operations available. Exiting.\n");
        return 1;
    }

    char input[256];
    printf("Calculator ready. Enter expression (e.g., 5 + 3) or 'exit':\n");

    while (1) {
        if (!fgets(input, sizeof(input), stdin) || strcmp(input, "exit\n") == 0) break;

        double a, b;
        char sign;
        if (sscanf(input, "%lf %c %lf", &a, &sign, &b) != 3) {
            printf("Invalid format. Use: Number Operation Number\n");
            continue;
        }

        int found = 0;
        for (int i = 0; i < op_count; i++) {
            if (pool[i].sign == sign) {
                double result = pool[i].function(a, b);
                printf("Result: %.2lf\n", result);
                found = 1;
                break;
            }
        }

        if (!found) printf("Operation '%c' is not supported.\n", sign);
    }

    for (int i = 0; i < op_count; i++) {
        FreeLibrary(pool[i].hLib);
    }

    return 0;
}