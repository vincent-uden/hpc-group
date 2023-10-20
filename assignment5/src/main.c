#define _XOPEN_SOURCE 700

#include <mpi.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "cli.h"
#include "read_input.h"
#include "diffusion.h"


int
main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int nmb_mpi_proc, mpi_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nmb_mpi_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    // Read args
    CliArgs args = parse_cli(argc, argv);

    // Read data
    int rows, cols;
    float *data = read_data(mpi_rank, nmb_mpi_proc, &rows, &cols)

    for ( int step = 0; step < args.n_iter; step++) {
        // Sync BC

        diffusion_step(data, rows, cols);
    }

    // Calculate average
    float internal_avg = 0.0f;
    for ( int i = 1; i <= rows; i ++) {
        for ( int j = 1; j <= cols; j++) {
            int index = i * (cols + 2) + j;
            internal_avg += data[index];
        }
    }
    internal_avg /= rows * cols;

    float avg;
    // Reduce all

    // Calculate abs diff
    float abs_diff;
    for ( int i = 1; i <= rows; i ++) {
        for ( int j = 1; j <= cols; j++) {
            int index = i * (cols + 2) + j;
            abs_diff += fabs(data[index] - avg);
        }
    }
    abs_diff /= rows * cols;

    // Reduce to mpi rank 0
    float reduce_abs_diff;

    if (mpi_rank == 0) {
        printf("average: %f\n", avg);
        printf("average absolute difference: %f\n", reduce_abs_diff);
    }

    return 0;
}
