#include "distances.h"

#ifdef TEST_DISTANCES
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    // Test Calc Dist
    printf("Testing calc dist linear...\n");
    for ( int i = 0; i < 10; i++) {
        Point p1, p2;
        p1.x = 0; p1.y = 0; p1.z = 0;
        p2.x = i; p2.y = 0; p2.z = 0;
        int dist = DIST_FUNC(&p1, &p2);
        if (dist == i) {
            printf("P");
        }
        else {
            printf("\nFailed dist %i: %i\n", i, dist);
        }
    }

    printf("\nTesting sqrt dist...\n");
    Point p1, p2;
    p1.x = 0; p1.y = 0; p1.z = 0;
    p2.x = 1; p2.y = 1; p2.z = 0;
    // sqrt(2)
    int dist = DIST_FUNC(&p1, &p2);
    if (dist == 1) {
        printf("P");
    }
    else {
        printf("\nFailed dist sqrt(2): %d\n", dist);
    }

    // sqrt(3)
    p2.z = 1;
    dist = DIST_FUNC(&p1, &p2);
    if (dist == 1) {
        printf("P");
    }
    else {
        printf("\nFailed dist sqrt(3): %d\n", dist);
    }

    // sqrt(300)
    p2.x = 10; p2.y = 10; p2.z = 10;
    dist = DIST_FUNC(&p1, &p2);
    if (dist == 17) {
        printf("P");
    }
    else {
        printf("\nFailed dist sqrt(300): %d\n", dist);
    }

    printf("\nTest bins assignment\n");
    int* bins = (int*) malloc(sizeof(int) * 10000);
    for (int i = 0; i < 10000; i++)
        bins[i] = 0;

    increment_bin(0, bins);
    increment_bin(10, bins);
    increment_bin(100, bins);
    increment_bin(1010, bins);
    increment_bin(1001, bins);
    increment_bin(2009, bins);
    increment_bin(567, bins);
    increment_bin(999, bins);
    increment_bin(67, bins);

    if (
        bins[0] == 1 &&
        bins[1] == 1 &&
        bins[10] == 1 &&
        bins[101] == 1 &&
        bins[201] == 1 &&
        bins[57] == 1 &&
        bins[100] == 2 &&
        bins[7] == 1
    )
    {
        printf("P");
    }
    else {
        printf("F");
    }

    printf("\n");
    return 0;
}
#endif
