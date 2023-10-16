#include <stdio.h>
#include <mpi.h>

#include "cli.h"
#include "read_input.h"

static inline void thrd_sender() {
}

static inline void thrd_receiver() {
}

int main(int argc, char **argv) {
    CliArgs args = parse_cli(argc, argv);

    MPI_Init(&argc, &argv);

    int mpi_threads, mpi_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    if (mpi_rank == 0) {
        printf("Running with %d threads\n", mpi_threads);

        thrd_sender();
    } else if (mpi_rank >= 1) {
        thrd_receiver();
    }

    return 0;
}
