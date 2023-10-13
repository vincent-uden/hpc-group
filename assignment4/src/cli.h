#ifndef CLI
#define CLI

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

typedef struct {
    size_t n_iter;
    float diff_c;
    int degree;
    int verbose;
} CliArgs;

CliArgs parse_cli(int argc, char** argv);

#endif
