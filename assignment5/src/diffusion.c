#include "diffusion.h"

void diffusion_step(double* prev_step, double* next_step, int rows, int cols, double c) {
    for (size_t i = 1; i <= rows; ++i) {
        for (size_t j = 1; j <= cols; ++j) {
            int index = i * (cols + 2) + j;
            next_step[index] = prev_step[index] + c * ((
                prev_step[index - 1] +
                prev_step[index + 1] +
                prev_step[index + cols + 2] +
                prev_step[index - cols - 2]
            ) / 4.0f - prev_step[index]);
        }
    }
}
