#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef double (*func)(double, double);

typedef struct {
	char sign;
	func function;
} Operation;

double a_sum(double a, double b) { return a + b; }
double a_diff(double a, double b) { return a - b; }
double a_mult(double a, double b) { return a * b; }
double a_div(double a, double b) { return a / b; }

Operation pool[] = {
	{'+', a_sum},
	{'-', a_diff},
	{'*', a_mult},
	{'/', a_div}
};