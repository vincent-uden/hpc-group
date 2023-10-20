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
    size_t rows, cols, global_rows;
    double *dataA = read_data(mpi_rank, nmb_mpi_proc, &rows, &cols, &global_rows);
    double *dataB = calloc((rows + 2)*(cols+2), sizeof(double));

    if (args.verbose && mpi_rank == 0)
        printf("Data read\n");

    for ( size_t step = 0; step < args.n_iter; step++) {
        double *data = step % 2 ? dataB : dataA;
        double *next_data = step % 2 ? dataA : dataB;

        if (args.verbose && mpi_rank == 0)
            printf("In step %lu\n", step);

        // Sync BC
        double *padding_up = data + 1;
        double *padding_down = data + (cols + 2) * (rows + 1) + 1;
        double *data_first = data + cols + 2 + 1;
        double *data_last = padding_down - cols - 2;

        MPI_Status status;
        if (nmb_mpi_proc > 1) {
            if ( mpi_rank == 0) {
                MPI_Sendrecv(data_last, cols, MPI_DOUBLE, 1, 0,
                            padding_down, cols, MPI_DOUBLE, 1, 0,
                            MPI_COMM_WORLD, &status);
            }
            else if ( mpi_rank == nmb_mpi_proc - 1) {
                MPI_Sendrecv(data_first, cols, MPI_DOUBLE, mpi_rank - 1, 0,
                            padding_up, cols, MPI_DOUBLE, mpi_rank - 1, 0,
                            MPI_COMM_WORLD, &status);
            }
            else {
                MPI_Sendrecv(data_first, cols, MPI_DOUBLE, mpi_rank - 1, 0,
                            padding_down, cols, MPI_DOUBLE, mpi_rank + 1, 0,
                            MPI_COMM_WORLD, &status);

                MPI_Sendrecv(data_last, cols, MPI_DOUBLE, mpi_rank + 1, 0,
                            padding_up, cols, MPI_DOUBLE, mpi_rank - 1, 0,
                            MPI_COMM_WORLD, &status);
            }
        }
        int a;
        MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);

        diffusion_step(data, next_data, rows, cols, args.diff_c);
    }

    if (args.verbose && mpi_rank == 0)
            printf("Loop done\n");

    double *data = args.n_iter % 2 ? dataB : dataA;

    // Calculate average
    double internal_avg = 0.0f;
    for ( size_t i = 1; i <= rows; i ++) {
        for ( int j = 1; j <= cols; j++) {
            int index = i * (cols + 2) + j;
            internal_avg += data[index];
        }
    }
    internal_avg /= global_rows * cols;

    // Reduce average to all processes
    double avg;
    MPI_Allreduce(&internal_avg, &avg, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    if (args.verbose && mpi_rank == 0)
        printf("Average calculated to %f\n", avg);

    // Calculate abs diff
    double abs_diff;
    for ( size_t i = 1; i <= rows; i ++) {
        for ( int j = 1; j <= cols; j++) {
            int index = i * (cols + 2) + j;
            abs_diff += fabs(data[index] - avg);
        }
    }
    abs_diff /= global_rows * cols;

    // Reduce to mpi rank 0
    double reduce_abs_diff;
    MPI_Reduce(&abs_diff, &reduce_abs_diff, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (args.verbose && mpi_rank == 0)
        printf("Abs diff Average calculated to %f\n", reduce_abs_diff);

    MPI_Finalize();

    free(dataA);
    free(dataB);

    if (mpi_rank == 0) {
        printf("average: %lf\n", avg);
        printf("average absolute difference: %lf\n", reduce_abs_diff);
    }

    return 0;
}
