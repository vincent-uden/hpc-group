#include "read_input.h"

void 
read_data(float *data, size_t rows, size_t cols, const char *filename) {
    // Create file pointer
    FILE *file = fopen(filename, "r");

    // Error handling 
	if(file==NULL) {
	 	printf("ERROR: could not find file init ...\n");
		return -1;
	}

    // Read the first row of the which is the size: [rows, cols]
	fscanf(file, "%d %d\n", &rows, &cols);
    printf("box [%d x %d]\n", rows, cols);

	double * data = (double*) malloc(rows*cols*sizeof(double));
	int x;
	int y;
	double val;
	while(fscanf(file, "%d %d %lf", &x, &y, &val) != EOF) {
		printf(" x y v = %d %d %lf\n", x, y, val);
		data[x*cols + y] = val;
	}
	
	fclose(file);
}

int main(int argc, char** argv) {

    double *data;
    size_t rows, cols;
    

   if (read_data(&data, &rows, &cols, "init") == 0) {
        // Use the 'data', 'rows', and 'cols' variables for further processing
        // Remember to free the allocated memory when you're done
        free(data);
    }
    return 0;
}