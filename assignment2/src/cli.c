#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

typedef struct {
    size_t threads;
    size_t chunk_size;
} CliArgs;

void
print_help(){
    printf("Usage: cell_distances -t <num_threads> -c <chunk_size>\n");
    printf("Options:\n");
    printf("  -t, --threads <num_threads>  Specify the number of threads (positive integer)\n");
    printf("  -c, --chunk_size <chunk_size> Specify the chunk size (positive integer)\n");
    printf("  -h, --help                   Display this help message\n");
}


CliArgs parse_cli(int argc, char** argv) {
    CliArgs args;

    // Default values
    args.threads = 1;
    args.chunk_size = 1000;

    // Define the long options
    struct option long_options[] = {
        // long_name , ?argument required, default, short_name
        {"threads",   0, 0, 't'},
        {"chunk_size", 0, 0, 'c'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}
    };

    int c;
    int opt_index = 0;
    while ((c = getopt_long(argc, argv, "t:c:?", long_options, &opt_index)) != -1) {
        switch (c) {
            case 't':
                args.threads = atoi(optarg);
                break;
            case 'c':
                args.chunk_size = atoi(optarg);
                break;
            case 'h':
                print_help();
                break;
            default :
                exit(EXIT_FAILURE);
        }
    }
    return args;
}

int main(int argc, char** argv) {
    CliArgs args = parse_cli(argc, argv);

    printf("Number of threads: %ld\n", args.threads);
    printf("Chunk size: %ld\n", args.chunk_size);

    return 0;
}
