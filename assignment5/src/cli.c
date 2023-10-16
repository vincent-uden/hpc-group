#include "cli.h"


CliArgs parse_cli(int argc, char** argv) {
    CliArgs args;

    // Default values
    args.n_iter = 1;
    args.diff_c = 1.0f;

    int c;
    int opt_index = 0;

    static struct option long_options[] = {
        // long_name , ?argument required, flag, short_name
        {"n_iterations",  required_argument , 0, 'n'},
        {"diffusion_c", required_argument, 0, 'd'},
    };

    while ((c = getopt_long(argc, argv, "n:d:", long_options, &opt_index)) != -1) {
        switch (c) {
            case 'n':
                args.n_iter = atoi(optarg);
                break;
            case 'd':
                args.diff_c = atof(optarg);
                break;
            default:
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

