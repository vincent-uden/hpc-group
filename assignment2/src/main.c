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

    Point* point_buffer = malloc(sizeof(Point) * args.chunk_size);
    char* read_buffer = malloc(ROW_LEN * args.chunk_size);
    int* bins = calloc(BINS, sizeof(int));

    if (point_buffer == NULL || read_buffer == NULL || bins == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        exit(1);
    }

    int rows_read;
    size_t start = 0;

    size_t current_point_i = 0;
    Point current_point;

    while (1) {
        rows_read = read_chunk(fp, current_point_i, 1, point_buffer, read_buffer);
        current_point = point_buffer[0];

        if (rows_read == 0) {
            if (args.verbose) {
                printf("Reached EOF at point %zu\n", current_point_i);
            }
            break;
        }

        start = current_point_i + 1;

        while ((rows_read = read_chunk(fp, start, args.chunk_size, point_buffer, read_buffer)) > 0) {
            #pragma omp parallel for
            for (size_t i = 0; i < rows_read; ++i) {
                if (args.verbose && i == 0) {
                    printf("Actually using: %d threads\n", omp_get_num_threads());
                }
                distance(&current_point, point_buffer + i, bins);
            }

            start += args.chunk_size;
        }

        current_point_i += 1;
    }

    free(point_buffer);
    free(read_buffer);
    fclose(fp);

    for (size_t i = 0; i < BINS; ++i) {
        if (bins[i] > 0) {
            printf("%02zu.%02zu %d\n", i / 100, i % 100, bins[i]);
        }
    }

    free(bins);

    return 0;
}
