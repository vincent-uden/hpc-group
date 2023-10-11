#include "slave.h"

Result **result;
char *write_queue;

// Slave Threads
int row_index;
int row_size;
mtx_t row_index_mtx;

// Master Thread
mtx_t work_mtx;
cnd_t work_done;
size_t next_row_to_write = 0;

void slave_main(void *_args){
    // Start working on rows
    while (row_index < row_size) {
        // Lock mutex for row index
        mtx_lock(&row_index_mtx);

         // Map im part [-2, 2], starting from 2 -> -2
        float im = 2.0 - row_index*4.0/row_size;
        float re; 

        // Loop to generate complex numbers for row index
		for (size_t j = 0; j < row_size; j++){

            // Map re part into [-2, 2], starting from -2 -> 2
            re = j*4.0/row_size - 2.0;

            // Define the complex number
			float complex z = re + im*I;

            // Compute newton 
			newton(z, result[row_index]);
		}

        // Increment row index
        if (row_index < row_size) {
            write_queue[row_index++] = 1;
            cnd_signal(&work_done);
        }
        // Unlock mutex 
        mtx_unlock(&row_index_mtx);
    }
}