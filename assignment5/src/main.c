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
    float *data = read_data(mpi_rank, nmb_mpi_proc, &rows, &cols);

    float *padding_up = data;
    float *padding_down = data + (cols + 2) * (rows + 1);
    float *data_first = data + cols + 2;
    float *data_last = padding_down - cols - 2;

    for ( int step = 0; step < args.n_iter; step++) {
        // Sync BC
        if (mpi_rank != 0 && mpi_rank != nmb_mpi_proc - 1) {
            MPI_Status status;
            MPI_Sendrecv(data_first, cols+2, MPI_FLOAT, mpi_rank - 1, 0,
                         padding_down, cols+2, MPI_FLOAT, mpi_rank + 1, 0,
                         MPI_COMM_WORLD, &status);

            MPI_Sendrecv(data_last, cols+2, MPI_FLOAT, mpi_rank + 1, 0,
                         padding_up, cols+2, MPI_FLOAT, mpi_rank - 1, 0,
                         MPI_COMM_WORLD, &status);
        }
        else if ( mpi_rank == 0) {
            MPI_Sendrecv(data_last, cols+2, MPI_FLOAT, 1, 0,
                         padding_down, cols+2, MPI_FLOAT, 1, 0,
                         MPI_COMM_WORLD, &status);
        }
        else if ( mpi_rank == nmb_mpi_proc - 1) {
            MPI_Sendrecv(data_first, cols+2, MPI_FLOAT, mpi_rank - 1, 0,
                         padding_up, cols+2, MPI_FLOAT, mpi_rank - 1, 0,
                         MPI_COMM_WORLD, &status);
        }

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

    // Reduce average to all processes
    float avg;
    MPI_Allreduce(&internal_avg, &avg, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

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
    MPI_Reduce(&abs_diff, &reduce_abs_diff, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    if (mpi_rank == 0) {
        printf("average: %f\n", avg);
        printf("average absolute difference: %f\n", reduce_abs_diff);
    }

    return 0;
}
