#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "point.h"
#include "read_file.h"
#include "distances.h"

#define BINS 3465

int main(int argc, char** argv) {

    CliArgs args = parse_cli(argc, argv);

    omp_set_num_threads(args.threads);
    if (args.verbose) {
        printf("Attempting to use %zu threads\n", args.threads);
    }

    FILE* fp = fopen(args.input_file, "r");

    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s\n", args.input_file);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    size_t file_size = ftell(fp);
    size_t n_chunks = (file_size - 1) / (ROW_LEN * args.chunk_size) + 1;

    Point* point_buffer_1 = malloc(sizeof(Point) * args.chunk_size);
    Point* point_buffer_2 = malloc(sizeof(Point) * args.chunk_size);
    char* read_buffer = malloc(ROW_LEN * args.chunk_size);

    int* bins = calloc(sizeof(int), BINS*args.threads);

    if (point_buffer_1 == NULL || point_buffer_2 == NULL || read_buffer == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        exit(1);
    }

    size_t rows_read;

    for (size_t c1 = 0; c1 < n_chunks; ++c1) {
        rows_read = read_chunk(fp, c1 * args.chunk_size, args.chunk_size, point_buffer_1, read_buffer);

        #pragma omp parallel for
        for (size_t i = 0; i < rows_read; ++i) {
            for (size_t j = i + 1; j < rows_read; ++j) {
                distance(point_buffer_1 + i, point_buffer_1 + j, bins + omp_get_thread_num() * BINS);
            }
        }

        for (size_t c2 = c1 + 1; c2 < n_chunks; ++c2) {
            rows_read = read_chunk(fp, c2 * args.chunk_size, args.chunk_size, point_buffer_2, read_buffer);

            #pragma omp parallel for
            for (size_t i = 0; i < args.chunk_size; ++i) {
                for (size_t j = 0; j < rows_read; ++j) {
                    distance(point_buffer_1 + i, point_buffer_2 + j, bins + omp_get_thread_num() * BINS);
                }
            }
        }
    }

    free(point_buffer_1);
    free(point_buffer_2);
    free(read_buffer);
    fclose(fp);

    int x;
    for (size_t i = 0; i < BINS; ++i) {
        x = 0;
        for ( int t = 0; t < args.threads; t++ ) x += bins[i+t*BINS];
        if (x > 0) {
            printf("%02zu.%02zu %d\n", i / 100, i % 100, x);
        }
    }

    return 0;
}
