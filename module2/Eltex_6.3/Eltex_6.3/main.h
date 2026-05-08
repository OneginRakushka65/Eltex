#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_PLUGINS 10

typedef double (*calc_func)(double, double);
typedef char (*sign_func)();

typedef struct {
    char sign;
    calc_func function;
    HMODULE hLib;
} Operation;

Operation pool[MAX_PLUGINS];
int op_count = 0;