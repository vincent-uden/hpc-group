#include "distances.h"

int calc_dist(Point* p1, Point* p2) {
    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;
    int dz = p2->z - p1->z;

    int dist_sq = dx*dx + dy*dy + dz*dz;
    return sqrt(dist_sq);
}

int calc_dist_float(Point* p1, Point* p2) {
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float dz = p2->z - p1->z;

    float dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

int calc_dist_float_point(PointFloat* p1, PointFloat* p2) {
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float dz = p2->z - p1->z;

    float dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

int calc_dist_double(Point* p1, Point* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double dz = p2->z - p1->z;

    double dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

int calc_dist_double_point(PointDouble* p1, PointDouble* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double dz = p2->z - p1->z;

    double dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

int calc_dist_binary_search(Point* p1, Point* p2) {
    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;
    int dz = p2->z - p1->z;

    int dist_sq = dx*dx + dy*dy + dz*dz;

    int low = 0;
    int high = MAX_SQ;

    while (low < high) {
        int m = (low + high) / 2;
        int sq = m*m;
        if ( sq == dist_sq ) return m;
        else if (sq > dist_sq) high = m - 1;
        else low = m + 1;
    }
    return low;
}

void increment_bin(int dist, int* bins) {
    // Increment the correct bin
    int bin = (dist + 5) / 10; // Truncate to 4 digits
    bins[bin] += 1;
}

void distance(Point* p1, Point* p2, int* bins) {
    // Calculate distance between point p1 and p2
    // and increase the bin with that distance
    int dist = calc_dist(p1, p2);
    increment_bin(dist, bins);
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
