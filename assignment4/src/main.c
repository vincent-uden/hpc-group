#include <stdio.h>
#include <stddef.h>
#include <string.h>
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#include "cli.h"
#include "read_input.h"

int
main(int argc, char **argv)
{
    cl_int error;

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
    cl_context_properties properties[] =
    {
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

    char *opencl_program_src;
    {
        FILE *clfp = fopen("./diffusion.cl", "r");
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
    
    cl_kernel kernel_diffusion_step = clCreateKernel(program, "diffusion_step", &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create kernel diffusion_step\n");
        return 1;
    }

    cl_kernel kernel_reduction = clCreateKernel(program, "reduction", &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create kernel reduction\n");
        return 1;
    }

    // ############### SETUP DONE, CODE START ###############

    // Read args
    CliArgs args = parse_cli(argc, argv);

    // Read input
    float *data;
    int rows, cols;
    read_data(data, &rows, &cols);

    // Setup GPU memory
    int mem_size = (rows + 2) * (cols + 2)
    cl_mem gpu_mem_a, gpu_mem_b;

    gpu_mem_a = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size*sizeof(cl_float), NULL, &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create gpu_mem_a c\n");
        return 1;
    }
    gpu_mem_b = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size*sizeof(cl_float), NULL, &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create gpu_mem_b c\n");
        return 1;
    }

    // Transfer data to GPU
    if ( clEnqueueWriteBuffer(command_queue, gpu_mem_a, CL_TRUE, 0, mem_size*sizeof(cl_float), data, 0, NULL, NULL) != CL_SUCCESS ) {
        fprintf(stderr, "cannot enqueue write of buffer data\n");
        return 1;
    }

    // Run diffusion steps on GPU
    const cl_float cl_c = (cl_float)args.diff_c;
    const cl_int cl_cols = (cl_int)cols;
    for ( int i = 0; i < args.n_iter; i++) {
        if ( i % 2 == 0 ) {
            clSetKernelArg(kernel_diffusion_step, 0, sizeof(cl_mem), &gpu_mem_a);
            clSetKernelArg(kernel_diffusion_step, 1, sizeof(cl_mem), &gpu_mem_b);
        }
        else {
            clSetKernelArg(kernel_diffusion_step, 0, sizeof(cl_mem), &gpu_mem_b);
            clSetKernelArg(kernel_diffusion_step, 1, sizeof(cl_mem), &gpu_mem_a);
        }
        clSetKernelArg(kernel_diffusion_step, 2, sizeof(cl_float), &cl_c);
        clSetKernelArg(kernel_diffusion_step, 3, sizeof(cl_int), &cl_cols);
        
        const size_t global_sz[] = {rows, cols};
        if ( clEnqueueNDRangeKernel(command_queue, kernel_diffusion_step,
                2, NULL, (const size_t*) &global_sz, NULL, 0, NULL, NULL)
            != CL_SUCCESS ) {
            fprintf(stderr, "cannot enqueue kernel diffusion step\n");
            return 1;
        }
    }

    if ( args.n_iter % 2 == 0) {
        if ( clEnqueueReadBuffer(command_queue,
            gpu_mem_a, CL_TRUE, 0, mem_size*sizeof(cl_float), data, 0, NULL, NULL)
            != CL_SUCCESS) {
            fprintf(stderr, "cannot enqueue read of buffer gpu_mem_a\n");
            return 1;
        }
    }
    else {
        if ( clEnqueueReadBuffer(command_queue,
            gpu_mem_b, CL_TRUE, 0, mem_size*sizeof(cl_float), data, 0, NULL, NULL)
            != CL_SUCCESS) {
            fprintf(stderr, "cannot enqueue read of buffer gpu_mem_a\n");
            return 1;
        }
    }

    if ( clFinish(command_queue) != CL_SUCCESS ) {
        fprintf(stderr, "cannot finish queue\n");
        return 1;
    }

    for ( int col = 1; col <= cols; col++) {
        for ( int row = 1; row <= rows; row++) {
            printf("%.3e ", data[row*cols + col]);
        }
        printf("\n");
    }

    // Free variables
    free(data);

    clReleaseMemObject(gpu_mem_a);
    clReleaseMemObject(gpu_mem_b);

    clReleaseProgram(program);
    clReleaseKernel(kernel_diffusion_step);
    clReleaseKernel(kernel_reduction);

    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    return 0;
}