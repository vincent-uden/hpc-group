#include "cli.h"


CliArgs parse_cli(int argc, char** argv) {
    CliArgs args;

    // Default values
    args.threads = 1;
    args.rows = 1000;
    args.degree = 3;
    args.verbose = 0;

    int c;
    int opt_index = 0;

    static struct option long_options[] = {
        // long_name , ?argument required, flag, short_name
        {"threads",  required_argument , 0, 't'},
        {"lines", required_argument, 0, 'l'},
        {"verbose", optional_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "t:l:vh", long_options, &opt_index)) != -1) {
        switch (c) {
            case 't':
                args.threads = atoi(optarg);
                break;
            case 'l':
                args.rows = atoi(optarg);
                break;
            case 'v':
                args.verbose = 1;
                break;
            case 'h':
                printf("Usage: newton [OPTIONS]\n");
                printf("Computes how complex plane points converge to function roots.\n");
                printf("\n");
                printf("Options:\n");
                printf("    -t, --threads <num_threads>   Specify the number of threads (positive integer), Default []\n");
                printf("    -l, --lines <lines>           Specify the number of rows Default []\n");
                printf("    -v, --verbose <verbose>      Use verbose output\n");
                printf("    -h, --help                    Display this help message\n");
                printf("\n");
                printf("Description:\n");
                printf("    This program computes how complex plane points converge to function roots\n");
                printf("    and plot two images: one colors each pixel based on the root of convergence,");
                printf("    while the other represents the number of iterations needed to approximate a root \n");
                printf("    The computation occurs in parallel using the C11 threads\n");
                printf("    You can specify the number of threads, rows and columns, and the degree of the polynomial f(x) = x^d-1 \n");
                printf("\n");
                printf("Example usage:\n");
                printf("    newton -t5 -l1000 3\n");
                exit(0);
                break;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Degree of the polynomial is missing.\n");
        exit(EXIT_FAILURE);
    }
    args.degree = atoi(argv[optind]);
    
    return args;
}


#ifdef CLI_MAIN

int main(int argc, char** argv) {
    CliArgs args = parse_cli(argc, argv);

    printf("\nNumber of threads: %ld\n", args.threads);
    printf("Chunk size: %ld\n", args.chunk_size);

    return 0;
}
#endif

