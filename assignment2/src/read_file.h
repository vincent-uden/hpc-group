#ifndef READ_FILE
#define READ_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "point.h"

#define ROW_LEN 24

void read_chunk(FILE* fp, size_t start, size_t points, Point* point_buffer, char* read_buffer);

#endif
