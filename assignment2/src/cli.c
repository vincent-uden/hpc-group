#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

typedef struct {
    size_t threads;
    size_t chunk_size;
} CliArgs;


CliArgs parse_cli(int argc, char** argv) {
    CliArgs args;

    // Default values
    args.threads = 1;
    args.chunk_size = 1000;
    
    int c;
    int opt_index = 0;

    static struct option long_options[] = {
        // long_name , ?argument required, default, short_name
        {"threads",  required_argument , 0, 't'},
        {"chunk_size", required_argument, 0, 'c'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "t:c:h", long_options, &opt_index)) != -1) {
        switch (c) {
            case 't':
                args.threads = atoi(optarg);
                break;
            case 'c':
                args.chunk_size = atoi(optarg);
                break;
            case 'h':
                printf("Usage: cell_distances -t <num_threads> -c <chunk_size>\n");
                printf("Options:\n");
                printf("  -t, --threads <num_threads>  Specify the number of threads (positive integer)\n");
                printf("  -c, --chunk_size <chunk_size> Specify the chunk size (positive integer)\n");
                printf("  -h, --help                   Display this help message\n");
                break;
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
