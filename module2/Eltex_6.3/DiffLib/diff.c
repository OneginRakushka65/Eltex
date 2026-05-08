#include <windows.h>

__declspec(dllexport) char get_sign() {
    return '-';
}

__declspec(dllexport) double calculate(double a, double b) {
    return a - b;
}