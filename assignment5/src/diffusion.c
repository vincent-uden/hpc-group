#include "diffusion.h"

void diffusion_step(float* prev_step, float* next_step, int rows, int cols, float c) {


    // Above
    for (size_t i = 0; i < rows; ++i) {
        float *read_row = prev_step + i * (cols + 2) + 1; 
        float *write_row = next_step + (i + 1) * (cols + 2) + 1;

        for (size_t j = 0; j < cols; ++j) {
            write_row[j] = read_row[j];
        }
    }

    // Below
    for (size_t i = 0; i < rows; ++i) {
        float *read_row = prev_step + (i + 2) * (cols + 2) + 1; 
        float *write_row = next_step + (i + 1) * (cols + 2) + 1;
        
        for (size_t j = 0; j < cols; ++j) {
            write_row[j] += read_row[j];
        }
    }

    // left
    for (size_t i = 0; i < rows; ++i) {
        float *read_row = prev_step + (i + 1) * (cols + 2); 
        float *write_row = next_step + (i + 1) * (cols + 2) + 1;
        
        for (size_t j = 0; j < cols; ++j) {
            write_row[j] += read_row[j];
        }
    }

    // right
    for (size_t i = 0; i < rows; ++i) {
        float *read_row = prev_step + (i + 1) * (cols + 2) + 2; 
        float *write_row = next_step + (i + 1) * (cols + 2) + 1;
        
        for (size_t j = 0; j < cols; ++j) {
            write_row[j] += read_row[j];
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
