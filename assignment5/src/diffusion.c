#include "diffusion.h"

void diffusion_step(float* prev_step, float* next_step, int rows, int cols, float c) {
    for (size_t i = 1; i <= rows; ++i) {
        float *curr_row = prev_step + i * (cols + 2);
        float *above_row = prev_step + (i - 1) * (cols + 2);
        float *below_row = prev_step + (i + 1) * (cols + 2);
        float *write_row = next_step + i * (cols + 2);

        for (size_t j = 1; j <= cols; ++j) {
            write_row[j] = curr_row[j] + c * ((
                curr_row[j - 1] +
                curr_row[j + 1] +
                below_row[j] +
                above_row[j]
            ) / 4.0f - curr_row[j]);
        }
    }
}
