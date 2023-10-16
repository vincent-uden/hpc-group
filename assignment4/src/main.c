#include <stdio.h>
#include <stddef.h>
#include <string.h>
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#include "cli.h"
#include "read_input.h"

int main(int argc, char **argv) {
    CliArgs args = parse_cli(argc, argv);

    cl_int error;

    // Attempt to acces the GPU and initialize the command queue
    cl_platform_id platform_id;
    cl_uint nmb_platforms;
    if ( clGetPlatformIDs(1, &platform_id, &nmb_platforms) != CL_SUCCESS ) {
        fprintf(stderr, "cannot get platform\n" );
        return 1;
    }

    cl_device_id device_id;
    cl_uint nmb_devices;
    if ( clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &nmb_devices) != CL_SUCCESS ) {
        fprintf(stderr, "cannot get device\n" );
        return 1;
    }

    cl_context context;
    cl_context_properties properties[] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties) platform_id,
        0
    };
    context = clCreateContext(properties, 1, &device_id, NULL, NULL, &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create context\n");
        return 1;
    }

    cl_command_queue command_queue;
    command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create command queue\n");
        return 1;
    }

    // "Compile" the OpenCL program
    char *opencl_program_src;
    {
        FILE *clfp = fopen("./src/diffusion.cl", "r");
        if ( clfp == NULL ) {
            fprintf(stderr, "could not load cl source code\n");
            return 1;
        }
        fseek(clfp, 0, SEEK_END);
        int clfsz = ftell(clfp);
        fseek(clfp, 0, SEEK_SET);
        opencl_program_src = (char*) malloc((clfsz+1)*sizeof(char));
        fread(opencl_program_src, sizeof(char), clfsz, clfp);
        opencl_program_src[clfsz] = 0;
        fclose(clfp);
    }

    cl_program program;
    size_t src_len = strlen(opencl_program_src);
    program = clCreateProgramWithSource(
                context, 1, (const char **) &opencl_program_src, (const size_t*) &src_len, &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create program\n");
        return 1;
    }

    free(opencl_program_src);

    error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot build program. log:\n");

        size_t log_size = 0;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        char *log = malloc(log_size*sizeof(char));
        if ( log == NULL ) {
          fprintf(stderr, "could not allocate memory\n");
          return 1;
        }

        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

        fprintf(stderr, "%s\n", log );

        free(log);

        return 1;
    }

    cl_kernel kernel = clCreateKernel(program, "diffusion_step", &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create kernel\n");
        return 1;
    }


    size_t rows = 10000;
    size_t cols = 10000;

    cl_mem gpu_a, gpu_b;
    gpu_a = clCreateBuffer(context, CL_MEM_READ_ONLY,
                       rows*cols * sizeof(double), NULL, &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create buffer a\n");
        return 1;
    }
    gpu_b = clCreateBuffer(context, CL_MEM_READ_ONLY,
                       rows*cols * sizeof(double), NULL, &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create buffer b\n");
        return 1;
    }

    double* cpu_a = read_data(&rows, &cols);
    double* cpu_b = calloc((rows+2)*(cols+2), sizeof(double));

    if ( clEnqueueWriteBuffer(command_queue, gpu_a, CL_TRUE, 0, (rows+2)*(cols+2) * sizeof(double), cpu_a, 0, NULL, NULL) != CL_SUCCESS ) {
        fprintf(stderr, "cannot enqueue write of buffer a\n");
        return 1;
    }
    if ( clEnqueueWriteBuffer(command_queue, gpu_b, CL_TRUE, 0, (rows+2)*(cols+2) * sizeof(double), cpu_b, 0, NULL, NULL) != CL_SUCCESS ) {
        fprintf(stderr, "cannot enqueue write of buffer b\n");
        return 1;
    }

    int cols_with_padding = cols + 2;

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &gpu_a);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &gpu_b);
    clSetKernelArg(kernel, 2, sizeof(double), &args.diff_c);
    clSetKernelArg(kernel, 3, sizeof(int), &cols_with_padding);

    const size_t global_sz[] = {rows, cols};
    if ( clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, (const size_t *) global_sz, NULL, 0, NULL, NULL) != CL_SUCCESS ) {
        fprintf(stderr, "cannot enqueue kernel\n");
        return 1;
    }

    if ( clFinish(command_queue) != CL_SUCCESS ) {
        fprintf(stderr, "cannot finish queue\n");
        return 1;
    }

    // TODO(Vincent): Reduction

    printf("Done!");

    free(cpu_a);
    free(cpu_b);

    clReleaseMemObject(gpu_a);
    clReleaseMemObject(gpu_b);

    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    return 0;
}
