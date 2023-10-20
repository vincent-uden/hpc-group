#define _XOPEN_SOURCE 700

#include <mpi.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

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



    return 0;
}
