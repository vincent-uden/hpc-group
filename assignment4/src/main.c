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
    // Read args
    CliArgs args = parse_cli(argc, argv);

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

    cl_kernel kernel_diff_abs = clCreateKernel(program, "diff_abs", &error);
    if ( error != CL_SUCCESS ) {
        fprintf(stderr, "cannot create kernel diff_abs\n");
        return 1;
    }

    // ############### SETUP DONE, CODE START ###############


    // Read input
    size_t rows, cols;
    // float *data = read_data(&rows, &cols);
    float *data = read_data(&rows, &cols);

    // Setup GPU memory
    size_t mem_size = (rows + 2) * (cols + 2);
    const int global_redsz = 1024;
    const int local_redsz = 32;
    const int nmb_redgps = global_redsz / local_redsz;
    cl_mem gpu_mem_a, gpu_mem_b, reduce_sum_mem;

    if (args.verbose) {
        printf("Using Kernel Size: %d\n", args.kernel_size);
    }

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
    reduce_sum_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, nmb_redgps*sizeof(cl_float), NULL, &error);
    if (error != CL_SUCCESS) {
        fprintf(stderr, "cannot create buffer reduce_sum_mem\n");
        return 1;
    }

    // Transfer data to GPU
    if ( clEnqueueWriteBuffer(command_queue, gpu_mem_a, CL_TRUE, 0, mem_size*sizeof(cl_float), data, 0, NULL, NULL) != CL_SUCCESS ) {
        fprintf(stderr, "cannot enqueue write of buffer data\n");
        return 1;
    }

    // Run diffusion steps on GPU
    const cl_float cl_c = (cl_float)args.diff_c;
    const cl_int cl_cols = (cl_int)(cols + 2);
    const int kernel_size = args.kernel_size;
    for ( size_t i = 0; i < args.n_iter; i++) {
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
        clSetKernelArg(kernel_diffusion_step, 4, sizeof(cl_int), &kernel_size);

        const size_t global_sz[] = {rows / (size_t)kernel_size, cols / (size_t)kernel_size};
        if ( clEnqueueNDRangeKernel(command_queue, kernel_diffusion_step,
                2, NULL, (const size_t*) &global_sz, NULL, 0, NULL, NULL)
            != CL_SUCCESS ) {
            fprintf(stderr, "cannot enqueue kernel diffusion step\n");
            return 1;
        }
    }

    cl_mem final_temps, final_abs_diff;
    if ( args.n_iter % 2 == 0) {
        final_temps = gpu_mem_a;
        final_abs_diff = gpu_mem_b;
    }
    else {
        final_temps = gpu_mem_b;
        final_abs_diff = gpu_mem_a;
    }

    // Reduce the sum on the GPU before transfering back to CPU
    const cl_int sz_clint = (cl_int)mem_size;
    clSetKernelArg(kernel_reduction, 0, sizeof(cl_mem), &final_temps);
    clSetKernelArg(kernel_reduction, 1, local_redsz*sizeof(cl_float), NULL);
    clSetKernelArg(kernel_reduction, 2, sizeof(cl_int), &sz_clint);
    clSetKernelArg(kernel_reduction, 3, sizeof(cl_mem), &reduce_sum_mem);

    size_t global_redsz_szt = (size_t) global_redsz;
    size_t local_redsz_szt = (size_t) local_redsz;
    if ( clEnqueueNDRangeKernel(command_queue,
            kernel_reduction, 1, NULL, (const size_t *) &global_redsz_szt, (const size_t *) &local_redsz_szt,
            0, NULL, NULL)
        != CL_SUCCESS) {
        fprintf(stderr, "cannot enqueue kernel reduction\n");
        return 1;
    }

    float *reduce_sum = malloc(nmb_redgps*sizeof(float));
    if ( clEnqueueReadBuffer(command_queue,
            reduce_sum_mem, CL_TRUE, 0, nmb_redgps*sizeof(cl_float), reduce_sum, 0, NULL, NULL)
        != CL_SUCCESS) {
        fprintf(stderr, "cannot enqueue read of reduce sum\n");
        return 1;
    }

    float avg_temp = 0.f;
    for (size_t ix = 0; ix < nmb_redgps; ++ix)
        avg_temp += reduce_sum[ix];
    avg_temp /= rows * cols;

    // Calculate the abs_diff from the average
    clSetKernelArg(kernel_diff_abs, 0, sizeof(cl_mem), &final_temps);
    clSetKernelArg(kernel_diff_abs, 1, sizeof(cl_mem), &final_abs_diff);
    clSetKernelArg(kernel_diff_abs, 2, sizeof(cl_float), &avg_temp);
    clSetKernelArg(kernel_diff_abs, 3, sizeof(cl_int), &cl_cols);

    const size_t global_sz[] = {rows, cols};
    if ( clEnqueueNDRangeKernel(command_queue, kernel_diff_abs,
            2, NULL, (const size_t*) &global_sz, NULL, 0, NULL, NULL)
        != CL_SUCCESS ) {
        fprintf(stderr, "cannot enqueue kernel diff_abs\n");
        return 1;
    }

    // Calculate the final average
    clSetKernelArg(kernel_reduction, 0, sizeof(cl_mem), &final_abs_diff);
    clSetKernelArg(kernel_reduction, 1, local_redsz*sizeof(cl_float), NULL);
    clSetKernelArg(kernel_reduction, 2, sizeof(cl_int), &sz_clint);
    clSetKernelArg(kernel_reduction, 3, sizeof(cl_mem), &reduce_sum_mem);

    if ( clEnqueueNDRangeKernel(command_queue,
            kernel_reduction, 1, NULL, (const size_t *) &global_redsz_szt, (const size_t *) &local_redsz_szt,
            0, NULL, NULL)
        != CL_SUCCESS) {
        fprintf(stderr, "cannot enqueue kernel reduction\n");
        return 1;
    }

    if ( clEnqueueReadBuffer(command_queue,
            reduce_sum_mem, CL_TRUE, 0, nmb_redgps*sizeof(cl_float), reduce_sum, 0, NULL, NULL)
        != CL_SUCCESS) {
        fprintf(stderr, "cannot enqueue read of reduce sum\n");
        return 1;
    }

    float abs_avg_temp = 0.f;
    for (size_t ix = 0; ix < nmb_redgps; ++ix)
        abs_avg_temp += reduce_sum[ix];
    abs_avg_temp /= rows * cols;

    if ( clFinish(command_queue) != CL_SUCCESS ) {
        fprintf(stderr, "cannot finish queue\n");
        return 1;
    }

    printf("average: %f\n", avg_temp);
    printf("average absolute difference: %f\n", abs_avg_temp);

    // Free variables
    free(data);
    free(reduce_sum);

    clReleaseMemObject(gpu_mem_a);
    clReleaseMemObject(gpu_mem_b);
    clReleaseMemObject(reduce_sum_mem);

    clReleaseProgram(program);
    clReleaseKernel(kernel_diffusion_step);
    clReleaseKernel(kernel_reduction);
    clReleaseKernel(kernel_diff_abs);

    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    return 0;
}
