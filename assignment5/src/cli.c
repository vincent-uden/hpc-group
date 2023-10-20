#include "cli.h"


CliArgs parse_cli(int argc, char** argv) {
    CliArgs args;

    // Default values
    args.n_iter = 1;
    args.diff_c = 1.0f;
    args.verbose = 0;
    args.print_rank = 0;

    int c;
    int opt_index = 0;

    static struct option long_options[] = {
        // long_name , ?argument required, flag, short_name
        {"n_iterations",  required_argument , 0, 'n'},
        {"diffusion_c", required_argument, 0, 'd'},
        {"verbose", optional_argument, 0, 'v'},
        {"print_rank", optional_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "n:d:vp:h", long_options, &opt_index)) != -1) {
        switch (c) {
            case 'n':
                args.n_iter = atoi(optarg);
                break;
            case 'd':
                args.diff_c = atof(optarg);
                break;
            case 'p':
                args.print_rank = atoi(optarg);
                break;
            case 'v':
                args.verbose = 1;
                break;
            case 'h':
                printf("Usage: diffusion [OPTIONS]\n");
                printf("Computes heat diffusion in 2-dimensional space.\n");
                printf("\n");
                printf("Options:\n");
                printf("    -n, --n_iterations <n_iterations>   Specify the number of threads (positive integer), Default []\n");
                printf("    -d, --diffusion_c <diffusion_c>     Specify the number of rows Default []\n");
                printf("    -v, --verbose <verbose>             Use verbose output\n");
                printf("    -h, --help                          Display this help message\n");
                printf("\n");
                printf("Description:\n");
                printf("    This program computes heat diffusion for n iterations with the diffusion constant c\n");
                printf("    The computation occurs in parallel using MPI\n");
                printf("    You can specify the number of iterations and the diffusion constant \n");
                printf("\n");
                printf("Example usage:\n");
                printf("    mpirun -n 10 diffusion -n20 -d0.02\n");
                exit(0);
                break;
        }
    }

    return args;
}


#ifdef CLI_MAIN

int main(int argc, char** argv) {
    CliArgs args = parse_cli(argc, argv);

    printf("Number of iterations: %ld\n", args.n_iter);
    printf("Diffusion constants: %f\n", args.diff_c);

    return 0;
}
#endif

