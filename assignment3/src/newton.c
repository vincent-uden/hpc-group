#include "newton.h"

float complex **roots;
const int up_to_deg = 10;

void precompute_roots() {
    roots = (float complex **) malloc(sizeof(float complex *) * (up_to_deg + 1));
    for ( int i = 1; i <= up_to_deg; i++) {
        float complex *roots_for_deg = (float complex *) malloc(sizeof(float complex) * i);
        for ( int j = 0; j < i; j++) {
            roots_for_deg[j] = cexpf(2.0f * I * M_PI * j / (float)i);
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

static inline int fast_abs(double complex z, double lower_bound) {
    return creal(z) * creal(z) + cimag(z) * cimag(z) < lower_bound * lower_bound;
}

void newton(double complex z, const int degree, Result *r) {
    for ( r->conv = 0, r->attr = INF_ATTR; r->conv < MAX_ITER; ++r->conv) {
        // Check stop cases
        if (fabs(creal(z)) > UPPER_BOUND | fabs(cimag(z)) > UPPER_BOUND) {
            // To big
            return;
        }
        if (fast_abs(z, LOWER_BOUND)) {
            // To small
            r->attr = ZERO_ATTR;
            return;
        }

        // Check close
        for ( int i = 0; i < degree; i++) {
            double complex zt = z - roots[degree][i];
            if (fast_abs(zt, LOWER_BOUND)) {
                r->attr = ZERO_ATTR + 1 + i;
                return;
            }
        }
        switch (degree)
        {
        case 1: z = CMPLX(1.0f, 0.0f); break;
        case 2: z = (0.5f * (z*z) + 0.5f) / z; break;
        case 3:
        {
            double complex z2 = z*z;
            z = ((2.0f / 3.0f) * z2*z + 1.0f / 3.0f) / z2;
            break;
        }
        case 4:
        {
            double complex z3 = z*z*z;
            z = ((3.0f/4.0f) * z3*z + 1.0f / 4.0f) / z3;
            break;
        }
        case 5:
        {
            double complex z2 = z*z;
            double complex z4 = z2*z2;
            z = ((4.0f/5.0f) * z4*z + 1.0f/5.0f) / z4;
            break;
        }
        case 6:
        {
            double complex z2 = z*z;
            double complex z5 = z2*z2*z;
            z = ((5.0f/6.0f) * z5*z + 1.0f/6.0f) / z5;
            break;
        }
        case 7:
        {
            double complex z2 = z*z;
            double complex z6 = z2*z2*z2;
            z = ((6.0f/7.0f) * z) + (1.0f/7.0f) / z6;
            break;
        }
        case 8:
        {
            double complex z2 = z*z;
            double complex z7 = z2*z2*z2*z;
            z = ((7.0f/8.0f) * z7*z + 1.0f/8.0f) / z7;
            break;
        }
        case 9:
        {
            double complex z2 = z*z;
            double complex z4 = z2*z2;
            double complex z8 = z4*z4;
            z = ((8.0f/9.0f) * z8*z + 1.0f/9.0f) / z8;
            break;
        }
        case 10:
        {
            double complex z3 = z*z*z;
            double complex z9 = z3*z3*z3;
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
    const int size = 2;
    float complex zs[2] = {
        CMPLXF(0.079, 0.00),
        CMPLXF(-0.01, 0.01)
    };

    precompute_roots();

    for ( int i = 0; i < size; ++i) {
        Result r;
        float complex z = zs[i];
        newton(z, 7, &r);

        printf("%.2f + %.2fi: attr: %d, conv: %d\n", creal(z), cimag(z), r.attr, r.conv);
    }

    free_roots();
}

#endif
