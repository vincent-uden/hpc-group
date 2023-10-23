#include "diffusion.h"

void diffusion_step(float* prev_step, float* next_step, int rows, int cols, float c) {

    // row major
    for (size_t i = 1; i <= rows; ++i) {
        float *read_row = prev_step + i * (cols + 2);
        float *read_row_above = prev_step + (i - 1) * (cols + 2) + 1;
        float *read_row_below = prev_step + (i + 1) * (cols + 2) + 1;
        float *write_row = next_step + i * (cols + 2);

        __builtin_prefetch(read_row, 0, 1);
        __builtin_prefetch(read_row_above, 0, 0);
        __builtin_prefetch(read_row_below, 0, 0);
        __builtin_prefetch(write_row, 1, 1);

        for (size_t j = 1; j <= cols; ++j) {
            write_row[j] = read_row[j] + c * ((read_row[j - 1] + read_row[j+1] + read_row_above[j] + read_row_below[j]) * 0.25 - read_row[j]);
        }
    }
}
