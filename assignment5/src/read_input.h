#ifndef READ_INPUT
#define READ_INPUT

#include <stdio.h>
#include <stdlib.h>

// Returns padded matrix, size of box
float * read_data(int mpi_rank, int nmb_mpi_proc, size_t *rows, size_t *cols);

#endif