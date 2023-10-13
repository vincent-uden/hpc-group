#include "slave.h"


int slave_main(void *_args){
    int slave_row;
    // Start working on rows
    while (row_index < middle_row) {
        // Lock mutex for row index
        mtx_lock(&row_index_mtx);

        // Another thread may have incremented row_index while we were waiting
        // for the lock.
        if (row_index >= middle_row) {
            mtx_unlock(&row_index_mtx);
            cnd_signal(&work_done);
            return 0;
        }

        slave_row = row_index;
        row_index++;
        mtx_unlock(&row_index_mtx);

         // Map im part [-2, 2], starting from 2 -> -2
        double im = 2.0 - slave_row*4.0/row_size;
        double re;

        // Loop to generate complex numbers for row index
		for (size_t j = 0; j < row_size; j++){

            // Map re part into [-2, 2], starting from -2 -> 2
            re = j*4.0/row_size - 2.0;

            // Define the complex number
			double complex z = re + im*I;

            // Compute newton
			newton(z, degree, result[slave_row] + j);
		}

        write_queue[slave_row] = 1;
        cnd_signal(&work_done);
    }

    return 0;
}
