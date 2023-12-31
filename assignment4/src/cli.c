#include "cli.h"


CliArgs parse_cli(int argc, char** argv) {
    CliArgs args;

    // Default values
    args.n_iter = 1;
    args.diff_c = 1.0f;
    args.verbose = 0;
    args.kernel_size = 1;

    int c;
    int opt_index = 0;

    static struct option long_options[] = {
        // long_name , ?argument required, flag, short_name
        {"n_iterations",  required_argument , 0, 'n'},
        {"diffusion_c", required_argument, 0, 'd'},
        {"verbose", optional_argument, 0, 'v'},
        {"kernel_size", optional_argument, 0, 'k'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "n:d:k:vh", long_options, &opt_index)) != -1) {
        switch (c) {
            case 'n':
                args.n_iter = atoi(optarg);
                break;
            case 'd':
                args.diff_c = atof(optarg);
                break;
            case 'k':
                args.kernel_size = atof(optarg);
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
                printf("    -k, --kernel_size <kernel_size>     Specify the size of the GPU kernels\n");
                printf("    -v, --verbose <verbose>             Use verbose output\n");
                printf("    -h, --help                          Display this help message\n");
                printf("\n");
                printf("Description:\n");
                printf("    This program computes heat diffusion for n iterations with the diffusion constant c\n");
                printf("    The computation occurs in parallel using the OpenCL\n");
                printf("    You can specify the number of iterations and the diffusion constant \n");
                printf("\n");
                printf("Example usage:\n");
                printf("    diffusion -n20 -d0.02\n");
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

