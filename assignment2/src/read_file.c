#include "read_file.h"
#include <stdio.h>

static int pow10_static[5] = {
    1, 10, 100, 1000, 10000,
};


// Assumes:
// - The file pointer is already open in reading mode
// - points < the length of buffer
// - read_buffer is at least 24 * points long
size_t read_chunk(FILE* fp, size_t start, size_t points, Point* point_buffer, char* read_buffer) {
    // There are 24 bytes in a row defining a point.
    fseek(fp, start * ROW_LEN, SEEK_SET);
    size_t rows = fread(read_buffer, sizeof(char), points * ROW_LEN, fp) / ROW_LEN;

    if (points < rows) {
        rows = points;
    }

    // Insider row
    Point tmp;

    // Inside pointer
    char component = 'x';

    // Inside component
    int sign = 1;
    int intra_comp_pos = 0;
    int comp = 0;

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < ROW_LEN; ++j) {
            switch (read_buffer[i * ROW_LEN + j]) {
            case ' ':
            case '\n':
                switch (component) {
                case 'x':
                    tmp.x = comp * sign;
                    break;
                case 'y':
                    tmp.y = comp * sign;
                    break;
                case 'z':
                    tmp.z = comp * sign;
                    break;
                }
                ++component;
                comp = 0;
                sign = 1;
                intra_comp_pos = 0;
                break;
            case '+':
                break;
            case '-':
                sign = -1;
                break;
            case '.':
                break;
            default:
                comp += pow10_static[(4 - intra_comp_pos)] * (read_buffer[i * ROW_LEN + j] - '0');
                ++intra_comp_pos;
                break;
            }
        }

        component = 'x';
        point_buffer[i] = tmp;
    }

    return rows;
}
