#include "read_input.h"

double min(double a, double b) {
    if (a < b) return a;
    return b;
}

double * read_data(int mpi_rank, int nmb_mpi_proc, size_t *rows, size_t *cols) {
    // Create file pointer
    FILE *file = fopen("init", "r");

    // Error handling
    if(file==NULL) {
        printf("ERROR: could not find file init ...\n");
        exit(1);
    }

    size_t global_rows;
    // Read the first row of the which is the size: [rows, cols]

    fscanf(file, "%zu %zu\n", &global_rows, cols);
    // printf("box [%zu x %zu]\n", *rows, *cols);
    
    // Calculate the number of rows each worker will process
    int rows_per_worker = global_rows / nmb_mpi_proc;
    int remainder_rows = global_rows % nmb_mpi_proc;
    
    int my_rows = (mpi_rank < remainder_rows) ? (rows_per_worker + 1) : rows_per_worker;
    *rows = my_rows;
    
    int row_zero = rows_per_worker * mpi_rank + min(mpi_rank, remainder_rows);

    
    // Initialize data matrix
    double *data = (double *)calloc((my_rows+2) * (*cols + 2), sizeof(double));

    if (data == NULL) {

        printf("Memory allocation failed.\n");
        fclose(file);
        exit(1);
    }

    // Initialize coordinates and val
    int x, y;
    double val;

    // Read values for each coordinates
    while(fscanf(file, "%d %d %f", &x, &y, &val) != EOF) {
        if (x >= row_zero && x < (row_zero + my_rows)){
            // printf(" x y v = %d %d %f\n", x, y, val);
            data[(x - row_zero + 1)*(*cols+2) + (y+1)] = val;
        }
    }
    
    fclose(file);

    return data;
}

#ifdef READ_INPUT_MAIN
int main(int argc, char** argv) {

    double *data;
    size_t rows, cols;
    int nmb_mpi_proc = 3;

    for (int mpi_rank = 0; mpi_rank < nmb_mpi_proc; ++mpi_rank ){
        data = read_data(mpi_rank, nmb_mpi_proc, &rows, &cols);

        for (size_t i = 0; i < (rows + 2) * (cols + 2); i++) {
            if (i % (cols + 2) == 0) {
                printf("\n");
            }
            printf("%.6f   ", data[i]);
        }
        printf("\n");
        printf("Rows: %d", rows);
    free(data);

    }
    return 0;
}
#endif
