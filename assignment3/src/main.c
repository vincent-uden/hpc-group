#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "cli.h"
#include "newton.h"


Result **result;
char *write_queue;

int row_index;
int row_size;

mtx_t row_index_mtx;
cnd_t work_done;


int main(int argc, char** argv) {

    CliArgs args = parse_cli(argc, argv);
    
    return 0;
}
