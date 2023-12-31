#ifndef CLI
#define CLI

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

typedef struct {
    size_t threads;
    size_t chunk_size;
    char* input_file;
    int verbose;
} CliArgs;

CliArgs parse_cli(int argc, char** argv);

#endif
