#ifndef PPM
#define PPM

#include <stdio.h>
#include <stdlib.h>

#include "newton.h"

char* generate_intensity_scheme(size_t n_levels);
char* generate_attractor_scheme(size_t n_colors);

void write_header(FILE* fp, int dims, int color_max);
void write_row(FILE* fp, size_t color_index, char* scheme);

#endif
