#ifndef SLAVE
#define SLAVE

#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include <complex.h>

#include "newton.h"

extern Result** result;
extern char *write_queue;
extern int row_index;
extern int row_size;
extern mtx_t row_index_mtx;
extern mtx_t work_mtx;
extern cnd_t work_done;
extern size_t next_row_to_write;
extern int degree;

int slave_main(void *_args);

#endif
