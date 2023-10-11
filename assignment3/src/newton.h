#ifndef NEWTON
#define NEWTON

#include <complex.h>
#include <stdint.h>
#include <math.h>

#define TYPE_ATTR uint8_t
#define TYPE_CONV uint8_t

typedef struct
{
    TYPE_ATTR attr;
    TYPE_CONV conv;
} Result;

void newton(float complex z, const int degree, Result *r);

#endif