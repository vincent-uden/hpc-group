#include "newton.h"


#define INF_ATTR 0
#define ZERO_ATTR 1
#define MAX_ITER 128
#define UPPER_BOUND 1e+10f
#define LOWER_BOUND 1e-3f


float complex **roots;
const int up_to_deg = 10;

void precompute_roots() {
    roots = (float complex **) malloc(sizeof(float complex *) * (up_to_deg + 1));
    for ( int i = 1; i <= up_to_deg; i++) {
        float complex *roots_for_deg = (float complex *) malloc(sizeof(float complex) * i);
        for ( int j = 0; j < i; j++) {
            roots_for_deg[j] = cexpf(I * M_PI * j / (float)i);
        }
        roots[i] = roots_for_deg;
    }
}

void free_roots() {
    for ( int i = 1; i <= up_to_deg; i++) {
        free(roots[i]);
    }
    free(roots);
}


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
    for ( r->conv = 0, r->attr = INF_ATTR; r->conv < MAX_ITER; ++r->conv) {

        // Check stop cases
        if (fabs(crealf(z)) > UPPER_BOUND | fabs(cimag(z)) > UPPER_BOUND) {
            // To big
            return;
        }
        if (cabsf(z) < LOWER_BOUND) {
            // To small
            r->attr = ZERO_ATTR;
            return;
        }

        // Check close
        for ( int i = 0; i < degree; i++) {
            float complex zt = z - roots[degree][i];
            if (cabsf(zt) < LOWER_BOUND) {
                r->attr = ZERO_ATTR + 1 + i;
                return;
            }
        }
        switch (degree)
        {
        case 1: z = CMPLXF(1.0f, 0.0f); break;
        case 2: z = (0.5f * (z*z) + 0.5f) / z; break;
        case 3: 
        {
            float complex z2 = z*z;
            z = ((2.0f / 3.0f) * z2*z + 1.0f / 3.0f) / z2;
            break;
        }
        case 4: 
        {
            float complex z3 = z*z*z;
            z = ((3.0f/4.0f) * z3*z + 1.0f / 4.0f) / z3;
            break;
        }
        case 5:
        {
            float complex z2 = z*z;
            float complex z4 = z2*z2;
            z = ((4.0f/5.0f) * z4*z + 1.0f/5.0f) / z4;
            break;
        }
        case 6:
        {
            float complex z2 = z*z;
            float complex z5 = z2*z2*z;
            z = ((5.0f/6.0f) * z5*z + 1.0f/6.0f) / z5;
            break;
        }
        case 7:
        {
            float complex z2 = z*z;
            float complex z6 = z2*z2*z2;
            z = ((6.0f/7.0f) * z6*z + 1.0f/7.0f) / z6;
            break;
        }
        case 8:
        {
            float complex z2 = z*z;
            float complex z7 = z2*z2*z2*z;
            z = ((7.0f/8.0f) * z7*z + 1.0f/8.0f) / z7;
            break;
        }
        case 9:
        {
            float complex z2 = z*z;
            float complex z4 = z2*z2;
            float complex z8 = z4*z4;
            z = ((8.0f/9.0f) * z8*z + 1.0f/9.0f) / z8;
            break;
        }
        case 10:
        {
            float complex z3 = z*z*z;
            float complex z9 = z3*z3*z3;
            z = ((9.0f/10.0f) * z9*z + 1.0f/10.0f) / z9;
            break;
        }
        default: exit(1); break;
        }
        
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

    precompute_roots();

    for ( int i = 0; i < size; ++i) {
        Result r;
        float complex z = zs[i];
        newton(z, 1, &r);

        printf("%.2f + %.2fi: attr: %d, conv: %d\n", creal(z), cimag(z), r.attr, r.conv);
    }

    free_roots();
}

#endif