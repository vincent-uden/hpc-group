#include "diffusion.h"

void diffusion_step(float* prev_step, float* next_step, int rows, int cols) {
    for (size_t i = 1; i < rows; ++i) {
        for (size_t j = 1; j < cols; ++j) {
            int index = i * (cols + 2) + j;
            next_step[index] = (
                prev_step[index - 1] +
                prev_step[index + 1] +
                prev_step[index + rows + 2] +
                prev_step[index - rows - 2]
            ) / 4.0f - prev_step[index];
        }
    }
}
