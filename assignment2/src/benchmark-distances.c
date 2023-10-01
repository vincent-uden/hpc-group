#include "point.h"
#include "distances.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define BENCHMARK(func, iters) \
    for (int i = 0; i < iters; ++i) { \
      func; \
    } \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime); \
    for (int i = 0; i < iters; ++i) { \
      func; \
    } \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime); \
    timeDiff = difftime(endTime.tv_sec, startTime.tv_sec) + (endTime.tv_nsec - startTime.tv_nsec) / 1e9; \
    printf("Time: %f\n", timeDiff); \

int main(int argc, char** argv) {

    // Timing variables
    double timeDiff = 0.0f;
    struct timespec startTime;
    struct timespec endTime;

    srand(time(NULL));

    const int test_size = 10000000;

    // Arrays
    Point* p1s = (Point*) malloc(sizeof(Point) * test_size);
    Point* p2s = (Point*) malloc(sizeof(Point) * test_size);
    PointFloat* p1fs = (PointFloat*) malloc(sizeof(PointFloat) * test_size);
    PointFloat* p2fs = (PointFloat*) malloc(sizeof(PointFloat) * test_size);
    PointDouble* p1ds = (PointDouble*) malloc(sizeof(PointDouble) * test_size);
    PointDouble* p2ds = (PointDouble*) malloc(sizeof(PointDouble) * test_size);
    volatile int* result = (int*) malloc(sizeof(int) * test_size);

    for (int i = 0; i < test_size; i++) {
        p1s[i].x = rand() % 20000 - 10000;
        p1s[i].y = rand() % 20000 - 10000;
        p1s[i].z = rand() % 20000 - 10000;
        p2s[i].x = rand() % 20000 - 10000;
        p2s[i].y = rand() % 20000 - 10000;
        p2s[i].z = rand() % 20000 - 10000;

        p1fs[i].x = rand() % 20000 - 10000;
        p1fs[i].y = rand() % 20000 - 10000;
        p1fs[i].z = rand() % 20000 - 10000;
        p2fs[i].x = rand() % 20000 - 10000;
        p2fs[i].y = rand() % 20000 - 10000;
        p2fs[i].z = rand() % 20000 - 10000;

        p1ds[i].x = rand() % 20000 - 10000;
        p1ds[i].y = rand() % 20000 - 10000;
        p1ds[i].z = rand() % 20000 - 10000;
        p2ds[i].x = rand() % 20000 - 10000;
        p2ds[i].y = rand() % 20000 - 10000;
        p2ds[i].z = rand() % 20000 - 10000;
    }

    printf("\n### Testing calc_dist ###\n");
    BENCHMARK(result[i] = calc_dist(&p1s[i], &p2s[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);

    printf("\n### Testing calc_dist_intrin ###\n");
    BENCHMARK(result[i] = calc_dist_intrin(&p1s[i], &p2s[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);

    printf("\n### Testing calc_dist_intrin_dot ###\n");
    BENCHMARK(result[i] = calc_dist_intrin_dot(&p1s[i], &p2s[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);

    printf("\n### Testing calc_dist_float ###\n");
    BENCHMARK(result[i] = calc_dist_float(&p1s[i], &p2s[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);

    printf("\n### Testing calc_dist_float_point ###\n");
    BENCHMARK(result[i] = calc_dist_float_point(&p1fs[i], &p2fs[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);

    printf("\n### Testing calc_dist_double ###\n");
    BENCHMARK(result[i] = calc_dist_double(&p1s[i], &p2s[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);

    printf("\n### Testing calc_dist_double_point ###\n");
    BENCHMARK(result[i] = calc_dist_double_point(&p1ds[i], &p2ds[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);
    
    printf("\n### Testing calc_dist_binary_search ###\n");
    BENCHMARK(result[i] = calc_dist_binary_search(&p1s[i], &p2s[i]), test_size)
    printf("Random resutl: %d\n", result[rand() % test_size]);

    return 0;
}