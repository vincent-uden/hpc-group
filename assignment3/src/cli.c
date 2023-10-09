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
        // long_name , ?argument required, default, short_name
        {"threads",  required_argument , 0, 't'},
        {"lines", required_argument, 0, 'l'},
        {"verbose", optional_argument, 0, 'v'},
        {"degree", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "t:l:d:vh", long_options, &opt_index)) != -1) {
        switch (c) {
            case 't':
                args.threads = atoi(optarg);
                break;
            case 'l':
                args.rows = atoi(optarg);
                break;
            case 'd':
                args.degree = atoi(optarg);
                break;
            case 'v':
                args.verbose = 1;
                break;
            case 'h':
                printf("Usage: cell_distances [OPTIONS]\n");
                printf("Calculate cell distances using parallization.\n");
                printf("\n");
                printf("Options:\n");
                printf("    -t, --threads <num_threads>   Specify the number of threads (positive integer), Default []\n");
                printf("    -l, --lines <lines>           Specify the number of rows Default []\n");
                printf("    -d, --degree <degree>         Specify the degree of the polynomial  Default []\n");
                printf("    -v, --verbose <versbose>      Use verbose output\n");
                printf("    -h, --help                    Display this help message\n");
                printf("\n");
                printf("Description:\n");
                printf("    This program calculates cell distances using multi-threading to speed up the process.\n");
                printf("    You can specify the number of threads and the chunk size to control the computation.\n");
                printf("\n");
                printf("Example usage:\n");
                printf("    cell_distances -t 4 -c 1000\n");
                exit(0);
                break;
        }
    }
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

