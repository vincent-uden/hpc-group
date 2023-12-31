#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "cli.h"
#include "newton.h"
#include "ppm.h"
#include "slave.h"


Result **result;
char *write_queue;

// Slave Threads
int row_index = 0;
int row_size;
int middle_row;
mtx_t row_index_mtx;

// Master Thread
mtx_t work_mtx;
cnd_t work_done;
size_t next_row_to_write = 0;

int degree = 0;

int main_slave_example(void* _arg) {
    sleep(2);
    while (row_index < row_size) {
        // Lock mutex
        mtx_lock(&row_index_mtx);

        // Increment row index
        if (row_index < row_size) {
            write_queue[row_index++] = 1;
            cnd_signal(&work_done);
        }

        // Unlock mutex
        mtx_unlock(&row_index_mtx);
    }
    return 0;
}

int main(int argc, char** argv) {
    CliArgs args = parse_cli(argc, argv);

    row_size = args.rows;
    degree = args.degree;
    middle_row = (args.rows + 1) / 2;
    if (args.rows % 2 == 0) {
        ++middle_row;
    }

    result = malloc(sizeof(Result*) * args.rows);
    for (size_t i = 0; i < args.rows; i++) {
        result[i] = malloc(sizeof(Result) * row_size);
    }

    thrd_t thrds[args.threads];
    write_queue = calloc(row_size, sizeof(char));

    mtx_init(&row_index_mtx, mtx_plain);

    mtx_init(&work_mtx, mtx_plain);
    cnd_init(&work_done);

    precompute_roots();

    // Create Threads
    int r = 0;
    for (size_t t = 0; t < args.threads; t++) {
        r = thrd_create(thrds + t, slave_main, NULL);

        if (r != thrd_success) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    // Create color schemes for writing
    char* intensity_scheme = generate_intensity_scheme(129);
    char* attractor_scheme = generate_attractor_scheme(args.degree + 2);

    char* intensity_path = malloc(sizeof(char) * 30);
    char* attractor_path = malloc(sizeof(char) * 30);
    sprintf(intensity_path, "newton_convergence_x%d.ppm", args.degree);
    sprintf(attractor_path, "newton_attractors_x%d.ppm", args.degree);
    FILE* intensity_file = fopen(intensity_path, "w");
    FILE* attractor_file = fopen(attractor_path, "w");

    write_header(intensity_file, args.rows, 129);
    write_header(attractor_file, args.rows, 255);

    // Manage threads and write to file
    mtx_lock(&work_mtx);
    while (next_row_to_write < middle_row) {
        cnd_wait(&work_done, &work_mtx);

        while (write_queue[next_row_to_write] == 1) {
            write_queue[next_row_to_write] = 2;
            for (size_t i = 0; i < row_size; ++i) {
                write_row(
                    intensity_file,
                    result[next_row_to_write][i].conv,
                    intensity_scheme
                );
                write_row(
                    attractor_file,
                    result[next_row_to_write][i].attr,
                    attractor_scheme
                );
            }
            next_row_to_write++;
        }
    }
    mtx_unlock(&work_mtx);

    int attr;
    while (next_row_to_write < args.rows) {
        for (size_t i = 0; i < row_size; ++i) {
            write_row(
                intensity_file,
                result[args.rows - next_row_to_write][i].conv,
                intensity_scheme
            );
            attr = result[args.rows - next_row_to_write][i].attr;
            if (attr > 2) {
                attr = args.degree + 4 - attr;
            }
            write_row(
                attractor_file,
                attr,
                attractor_scheme
            );
        }
        next_row_to_write++;
    }

    // Clean up
    for (size_t t = 0; t < args.threads; t++) {
        thrd_join(thrds[t], NULL);
    }

    for (size_t i = 0; i < args.rows; i++) {
        free(result[i]);
    }
    free_roots();
    free(result);
    free(write_queue);
    free(intensity_scheme);
    free(attractor_scheme);
    fclose(intensity_file);
    fclose(attractor_file);
    mtx_destroy(&row_index_mtx);
    cnd_destroy(&work_done);

    return 0;
}
