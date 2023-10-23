#include "diffusion.h"

void diffusion_step(float* prev_step, float* next_step, int rows, int cols, float c) {

    // Zero initialize first row
    float *first_row = next_step + cols + 2;
    for ( size_t i = 1; i <= rows; ++i) {
        first_row[i] = 0.0f;
    }

    // Above
    for (size_t i = 1; i <= rows; ++i) {
        float *read_row = prev_step + i * (cols + 2) + 1;

        float *write_above = next_step + (i - 1) * (cols + 2) + 1;
        float *write_below = next_step + (i + 1) * (cols + 2) + 1;
        float *write_left = next_step + i * (cols + 2);
        float *write_right = next_step + i * (cols + 2) + 2;

        __builtin_prefetch(read_row, 0, 0);
        __builtin_prefetch(write_above, 1, 0);
        __builtin_prefetch(write_below, 1, 0);
        __builtin_prefetch(write_left, 1, 0);
        __builtin_prefetch(write_right, 1, 0);

        for (size_t j = 0; j < cols; ++j) {
            write_below[j] = read_row[j] * (i != rows);
            write_left[j] += read_row[j] * (j != 0);
            write_right[j] += read_row[j] * (j != cols - 1);
            write_above[j] += read_row[j] * (i != 1);
        }
    }

    // final loop
    for (size_t i = 0; i < rows; ++i) {
        float *prev_row = prev_step + (i + 1) * (cols + 2) + 1;
        float *next_row = next_step + (i + 1) * (cols + 2) + 1;
        
        for (size_t j = 0; j < cols; ++j) {
            next_row[j] = prev_row[j] + c * (next_row[j] * 0.25 - prev_row[j]);
        }
    }
}
