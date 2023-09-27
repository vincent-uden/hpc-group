#include "../src/read_file.h"
#include "../src/point.h"
#include <string.h>

int read_chunk_parses_small_file_correctly() {
    char* path = "./test/small_sample.txt";

    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Could not open file %s\n", path);
        return 1;
    }

    Point expected[] = {
        {.x = +1330, .y = -9035, .z = +3489},
        {.x = -3718, .y = +2517, .z = -5995},
        {.x = +9568, .y = -3464, .z = +2645},
        {.x = -9620, .y = +9279, .z = +8828},
        {.x = +7630, .y = -2290, .z = +679},
        {.x = +4113, .y = -3399, .z = +5299},
        {.x = -994,  .y = +7313, .z = -6523},
        {.x = +3376, .y = -3614, .z = -6657},
        {.x = +1304, .y = +9381, .z = -1559},
        {.x = -4238, .y = -7514, .z = +8942},
    };

    Point* actual = malloc(sizeof(Point) * 10);
    memset(actual, 0, sizeof(Point) * 10);

    char* bfr = malloc(sizeof(char) * 10 * 24);
    memset(bfr, 0, sizeof(char) * 10 * 24);

    read_chunk(fp, 0, 10, actual, bfr);

    for (int i = 0; i < 10; i++) {
        if (actual[i].x!= expected[i].x ||
            actual[i].y!= expected[i].y ||
            actual[i].z!= expected[i].z) {
            return 1;
        }
    }

    free(actual);
    free(bfr);

    return 0;
}

int read_chunk_parses_partial_file_correctly() {
    char* path = "./test/small_sample.txt";

    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Could not open file %s\n", path);
        return 1;
    }

    Point expected[] = {
        {.x = -3718, .y = +2517, .z = -5995},
        {.x = +9568, .y = -3464, .z = +2645},
        {.x = -9620, .y = +9279, .z = +8828},
        {.x = +7630, .y = -2290, .z = +679},
        {.x = +4113, .y = -3399, .z = +5299},
    };

    Point* actual = malloc(sizeof(Point) * 10);
    memset(actual, 0, sizeof(Point) * 10);

    char* bfr = malloc(sizeof(char) * 10 * 24);
    memset(bfr, 0, sizeof(char) * 10 * 24);

    read_chunk(fp, 1, 5, actual, bfr);

    for (int i = 0; i < 10; i++) {
        if (actual[i].x!= expected[i].x ||
            actual[i].y!= expected[i].y ||
            actual[i].z!= expected[i].z) {
            return 1;
        }
    }

    free(actual);
    free(bfr);

    return 0;
}

char* boolToStr(int b) {
    if (!b) {
        return "YES";
    } else {
        return "NO";
    }
}

int main(int argc, char** argv) {
    int (*tests[])() = {
        read_chunk_parses_small_file_correctly,
        read_chunk_parses_small_file_correctly,
    };
    char* testNames[] = {
        "Read chunk correctly parses a small file: %s\n",
        "Read chunk correctly parses a part of a file: %s\n",
    };

    int total = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    int result;

    printf("====================== Running Tests =============================\n");
    for (int i = 0; i < total; i++) {
        result = (*tests[i])();
        passed += !result;
        printf(testNames[i], boolToStr(result));
    }
    printf("\n%d/%d tests passed.\n", passed, total);
    printf("==================================================================\n");
}
