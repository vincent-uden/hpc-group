#ifndef NEWTON
#define NEWTON

#include <complex.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define TYPE_ATTR uint8_t
#define TYPE_CONV uint8_t


#define INF_ATTR 0
#define ZERO_ATTR 1
#define MAX_ITER 128
#define UPPER_BOUND 1e+10f
#define LOWER_BOUND 1e-3f
#define PI 3.14159265358979323846

typedef struct
{
    TYPE_ATTR attr;
    TYPE_CONV conv;
} Result;

void precompute_roots();
void free_roots();
void newton(double complex z, const int degree, Result *r);

#endif
