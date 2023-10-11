#include "newton.h"


#define INF_ATTR 0
#define ZERO_ATTR 1
#define MAX_ITER 128
#define UPPER_BOUND 1e+10f
#define LOWER_BOUND 1e-3f


void newton1(float complex z, Result *r) {
    for ( r->conv = 0, r->attr = INF_ATTR; r->conv < MAX_ITER; ++r->conv) {
        if (fabs(crealf(z)) > UPPER_BOUND | fabs(cimag(z)) > UPPER_BOUND) {
            return;
        }
        if (cabsf(z) < LOWER_BOUND) {
            r->attr = ZERO_ATTR;
            return;
        }
        float complex zp = z - 1;
        if (cabsf(zp) < LOWER_BOUND) {
            r->attr = ZERO_ATTR + 1;
            return;
        }
        z = CMPLXF(1.0f, 0.0f);
    }
}

void newton2(float complex z, Result *r) {
    for ( r->conv = 0, r->attr = INF_ATTR; r->conv < MAX_ITER; ++r->conv) {
        if (fabs(crealf(z)) > UPPER_BOUND | fabs(cimag(z)) > UPPER_BOUND) {
            return;
        }
        if (cabsf(z) < LOWER_BOUND) {
            r->attr = ZERO_ATTR;
            return;
        }

        {
            // Check both at the same time by taking abs(real(z))
            float complex zt = CMPLXF(fabs(creal(z)), cimag(z));
            float complex zp = zt - 1;
            if (cabsf(zp) < LOWER_BOUND) {
                r->attr = ZERO_ATTR + 1;
                return;
            }
        }
        z = ((z*z) + 1) / (2 * z);
    }
}

void newton(float complex z, const int degree, Result *r) {
    switch (degree)
    {
    case 1:
        newton1(z, r);
        break;
    
    default:
        break;
    }
}

#ifdef NEWTON_MAIN

#include <stdio.h>
int main(int, char **) {
    const int size = 4;
    float complex zs[size] = {
        CMPLXF(2.0, -2.0),
        CMPLXF(0.0, 0.0),
        CMPLXF(UPPER_BOUND * 1.1, UPPER_BOUND),
        CMPLXF(1.0, 0.0),
    };

    for ( int i = 0; i < size; ++i) {
        Result r;
        float complex z = zs[i];
        newton(z, 1, &r);

        printf("%.2f + %.2fi: attr: %d, conv: %d\n", creal(z), cimag(z), r.attr, r.conv);
    }
}

#endif