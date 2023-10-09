#ifndef NEWTON
#define NEWTON

#include <complex.h>

#define TYPE_ATTR char
#define TYPE_CONV char

typedef struct
{
    TYPE_ATTR attr;
    TYPE_CONV conv;
} Result;

void newton(float complex z, Result *r);

#endif