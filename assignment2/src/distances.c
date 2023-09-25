#include <math.h>

#include "point.h"

#define MAX_SQ 20000*20000 * 3

int _calc_dist(Point* p1, Point* p2) {
    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;
    int dz = p2->z - p1->z;

    int dist_sq = dx*dx + dy*dy + dz*dz;
    return sqrt(dist_sq);
}

int calc_dist(Point* p1, Point* p2) {
    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;
    int dz = p2->z - p1->z;

    int dist_sq = dx*dx + dy*dy + dz*dz;

    int low = 0;
    int high = MAX_SQ;

    while (low < high) {
        int m = (low + high) / 2;
        int sq = m*m;
        if (sq < dist_sq) low = m + 1;
        else high = m - 1;
    }
    return low;
}

void increment_bin(int dist, int* bins) {
    // Increment the correct bin
    int bin = dist / 10; // Truncate 4 digits
    bins[bin] += 1;
}

void distance(Point* p1, Point* p2, int* bins) {
    // Calculate distance between point p1 and p2
    // and increase the bin with that distance
    int dist = calc_dist(p1, p2);
}

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
        int dist = calc_dist(&p1, &p2);
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
    int dist = calc_dist(&p1, &p2);
    if (dist == 1) {
        printf("P");
    }
    else {
        printf("\nFailed dist sqrt(2): %d\n", dist);
    }

    // sqrt(3)
    p2.z = 1;
    dist = calc_dist(&p1, &p2);
    if (dist == 1) {
        printf("P");
    }
    else {
        printf("\nFailed dist sqrt(3): %d\n", dist);
    }

    // sqrt(300)
    p2.x = 10; p2.y = 10; p2.z = 10;
    dist = calc_dist(&p1, &p2);
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
        bins[100] == 1 &&
        bins[200] == 1 &&
        bins[56] == 1 &&
        bins[99] == 1 &&
        bins[6] == 1
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

#ifdef BENCHMARK_DISTANCES
#include <stdio.h>
#include <time.h>
double timeDiff = 0.0f;
struct timespec startTime;
struct timespec endTime;

#define BENCHMARK(func, iters) \
    timespec_get(&startTime, TIME_UTC); \
    for (int i = 0; i < iters; ++i) { \
      func; \
    } \
    timespec_get(&endTime, TIME_UTC); \
    timeDiff = difftime(endTime.tv_sec, startTime.tv_sec) + (endTime.tv_nsec - startTime.tv_nsec) / 1e9; \
    printf("Time: %f\n", timeDiff); \

int main(int argc, char** argv) {
    Point p1, p2;
    p1.x = 1543; p1.y = 1235; p1.z = 14385;
    p2.x = 145; p2.y = 2346; p2.z = 257;
    volatile int dist;
    BENCHMARK(dist = calc_dist(&p1, &p2), 1000)

    BENCHMARK(dist = calc_dist(&p1, &p2), 1000000)

    BENCHMARK(dist = calc_dist(&p1, &p2), 1000000000)

    printf("dist: %d\n", dist);
    return 0;
}
#endif
