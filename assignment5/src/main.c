#define _XOPEN_SOURCE 700

#include <mpi.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "cli.h"
#include "read_input.h"
#include "diffusion.h"

void printdata(float* data, int rows, int cols, int mpi_rank) {
    printf("Rank %d: \n", mpi_rank);
    for (int i = 0; i < rows + 2; ++i) {
        for (int j = 0; j < cols + 2; ++j) {
            printf("%lf ", data[i * (cols+2) + j]);
        }
        printf("\n");
    }
    printf("\n");
}

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
    size_t rows, cols, global_rows;
    float *dataA = read_data(mpi_rank, nmb_mpi_proc, &rows, &cols, &global_rows);
    float *dataB = calloc((rows + 2)*(cols+2), sizeof(float));

    if (args.verbose && mpi_rank == 0)
        printf("Data read\n");

    for ( size_t step = 0; step < args.n_iter; step++) {
        float *data = step % 2 ? dataB : dataA;
        float *next_data = step % 2 ? dataA : dataB;

        if (args.verbose && mpi_rank == 0)
            printf("In step %lu\n", step);

        // Sync BC
        float *padding_up = data + 1;
        float *padding_down = data + (cols + 2) * (rows + 1) + 1;
        float *data_first = data + cols + 2 + 1;
        float *data_last = padding_down - cols - 2;

        MPI_Status status;
        if (nmb_mpi_proc > 1 && mpi_rank % 2 == 0) {
            if ( mpi_rank == 0) {
                MPI_Sendrecv(data_last, cols, MPI_FLOAT, 1, step,
                            padding_down, cols, MPI_FLOAT, 1, step,
                            MPI_COMM_WORLD, &status);
            }
            else if ( mpi_rank == nmb_mpi_proc - 1) {
                MPI_Sendrecv(data_first, cols, MPI_FLOAT, mpi_rank - 1, step,
                            padding_up, cols, MPI_FLOAT, mpi_rank - 1, step,
                            MPI_COMM_WORLD, &status);
            }
            else {
                MPI_Sendrecv(data_first, cols, MPI_FLOAT, mpi_rank - 1, step,
                            padding_down, cols, MPI_FLOAT, mpi_rank + 1, step,
                            MPI_COMM_WORLD, &status);

                MPI_Sendrecv(data_last, cols, MPI_FLOAT, mpi_rank + 1, step,
                            padding_up, cols, MPI_FLOAT, mpi_rank - 1, step,
                            MPI_COMM_WORLD, &status);
            }
        }

        diffusion_step(data, next_data, rows, cols, args.diff_c);
    }

    if (args.verbose && mpi_rank == 0)
            printf("Loop done\n");

    float *data = args.n_iter % 2 ? dataB : dataA;

    // Calculate average
    float internal_avg = 0.0f;
    for ( size_t i = 1; i <= rows; i ++) {
        for ( int j = 1; j <= cols; j++) {
            int index = i * (cols + 2) + j;
            internal_avg += data[index];
        }
    }
    internal_avg /= global_rows * cols;

    // Reduce average to all processes
    float avg;
    MPI_Allreduce(&internal_avg, &avg, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

    if (args.verbose && mpi_rank == 0)
        printf("Average calculated to %f\n", avg);

    // Calculate abs diff
    float abs_diff;
    for ( size_t i = 1; i <= rows; i ++) {
        for ( int j = 1; j <= cols; j++) {
            int index = i * (cols + 2) + j;
            abs_diff += fabs(data[index] - avg);
        }
    }
    abs_diff /= global_rows * cols;

    // Reduce to mpi rank 0
    float reduce_abs_diff;
    MPI_Reduce(&abs_diff, &reduce_abs_diff, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (args.verbose && mpi_rank == 0)
        printf("Abs diff Average calculated to %f\n", reduce_abs_diff);

    MPI_Finalize();

    free(dataA);
    free(dataB);

    if (mpi_rank == 0) {
        printf("average: %f\n", avg);
        printf("average absolute difference: %f\n", reduce_abs_diff);
    }

    return 0;
}
