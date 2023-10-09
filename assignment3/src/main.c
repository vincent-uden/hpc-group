#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "cli.h"
#include "newton.h"


Result **result;
char *write_queue;

int row_index;
int row_size;

mtx_t row_index_mtx;
cnd_t work_done;

int main_slave(void* _arg) {
    printf("Slave thread started\n");

    while (row_index < row_size) {
        // Lock mutex
        mtx_lock(&row_index_mtx);

        // Increment row index
        if (row_index < row_size) {
            printf("Row index: %d\n", row_index);
            row_index++;
        }

        // Unlock mutex
        mtx_unlock(&row_index_mtx);
    }

    return 0;
}

int main(int argc, char** argv) {
    CliArgs args = parse_cli(argc, argv);

    row_size = args.rows;

    result = malloc(sizeof(Result*) * args.rows);
    for (size_t i = 0; i < args.rows; i++) {
        result[i] = malloc(sizeof(Result) * row_size);
    }

    thrd_t thrds[args.threads];
    write_queue = malloc(sizeof(char) * row_size);

    mtx_init(&row_index_mtx, mtx_plain);
    cnd_init(&work_done);

    int r = 0;
    for (size_t t = 0; t < args.threads; t++) {
        printf("Creating thread\n");
        r = thrd_create(thrds + t, main_slave, NULL);

        if (r != thrd_success) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    // Clean up
    for (size_t t = 0; t < args.threads; t++) {
        thrd_join(thrds[t], NULL);
    }

    for (size_t i = 0; i < args.rows; i++) {
        free(result[i]);
    }
    free(result);
    free(write_queue);
    mtx_destroy(&row_index_mtx);
    cnd_destroy(&work_done);

    return 0;
}
