#include "read_input.h"

float * read_data(size_t *rows, size_t *cols) {
    // Create file pointer
    FILE *file = fopen("init", "r");

    // Error handling
	if(file==NULL) {
	 	printf("ERROR: could not find file init ...\n");
        exit(1);
	}

    // Read the first row of the which is the size: [rows, cols]
	fscanf(file, "%zu %zu\n", rows, cols);
    // printf("box [%zu x %zu]\n", *rows, *cols);

    // Initialize data matrix
    float *data = (float *)calloc((*rows + 2) * (*cols + 2), sizeof(float));

    if (data == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        exit(1);
    }

    // Initialize coordinates and val
    int x, y;
	float val;

    // Read values for each coordinates
	while(fscanf(file, "%d %d %f", &x, &y, &val) != EOF) {
		// printf(" x y v = %d %d %f\n", x, y, val);
		data[(x+1)*(*cols+2) + (y+1)] = val;
	}

	fclose(file);

    return data;
}

#ifdef READ_INPUT_MAIN
int main(int argc, char** argv) {

    float *data;
    size_t rows, cols;

    data = read_data(&rows, &cols);

    for (size_t i = 0; i < (rows + 2) * (cols + 2); i++) {
        if (i % (cols + 2) == 0) {
            printf("\n");
        }
        printf("%.6f   ", data[i]);
    }

    printf("\nRows: %zu\n", rows);
    printf("Cols: %zu \n", cols);
    printf("Data_element: %.6f", data[12]);

    free(data);
    return 0;
}
#endif
