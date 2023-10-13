#include <stdio.h>
#include <stddef.h>
#include <string.h>
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

int
main()
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
  
  cl_kernel kernel_step = clCreateKernel(program, "step", &error);
  if ( error != CL_SUCCESS ) {
    fprintf(stderr, "cannot create kernel step\n");
    return 1;
  }

  cl_kernel kernel_reduction = clCreateKernel(program, "reduction", &error);
  if ( error != CL_SUCCESS ) {
    fprintf(stderr, "cannot create kernel reduction\n");
    return 1;
  }

  // TODO: Change everything below this point

  const int sz = 10000000;
  const int global_redsz = 1024;
  const int local_redsz = 32;
  const int nmb_redgps = global_redsz / local_redsz;

  cl_mem input_buffer_a, input_buffer_b, output_buffer_c, output_buffer_c_sum;
  input_buffer_a = clCreateBuffer(context, CL_MEM_READ_ONLY, sz*sizeof(float), NULL, &error);
  if ( error != CL_SUCCESS ) {
    fprintf(stderr, "cannot create buffer a\n");
    return 1;
  }
  input_buffer_b = clCreateBuffer(context, CL_MEM_READ_ONLY, sz*sizeof(float), NULL, &error);
  if ( error != CL_SUCCESS ) {
    fprintf(stderr, "cannot create buffer b\n");
    return 1;
  }
  output_buffer_c = clCreateBuffer(context, CL_MEM_READ_WRITE, sz*sizeof(float), NULL, &error);
  if ( error != CL_SUCCESS ) {
    fprintf(stderr, "cannot create buffer c\n");
    return 1;
  }
  output_buffer_c_sum = clCreateBuffer(context, CL_MEM_WRITE_ONLY, nmb_redgps*sizeof(float), NULL, &error);
  if (error != CL_SUCCESS) {
    fprintf(stderr, "cannot create buffer c_sum\n");
    return 1;
  }

  float *a = malloc(sz*sizeof(float));
  float *b = malloc(sz*sizeof(float));
  for ( int ix = 0; ix < sz; ++ix ) {
    a[ix] = ix;
    b[ix] = ix;
  }
  if ( clEnqueueWriteBuffer(command_queue,
           input_buffer_a, CL_TRUE, 0, sz*sizeof(float), a, 0, NULL, NULL)
       != CL_SUCCESS ) {
    fprintf(stderr, "cannot enqueue write of buffer a\n");
    return 1;
  }
  if ( clEnqueueWriteBuffer(command_queue,
           input_buffer_b, CL_TRUE, 0, sz*sizeof(float), b, 0, NULL, NULL)
       != CL_SUCCESS ) {
    fprintf(stderr, "cannot enqueue write of buffer b\n");
    return 1;
  }
  clSetKernelArg(kernel_dot_prod_mul, 0, sizeof(cl_mem), &input_buffer_a);
  clSetKernelArg(kernel_dot_prod_mul, 1, sizeof(cl_mem), &input_buffer_b);
  clSetKernelArg(kernel_dot_prod_mul, 2, sizeof(cl_mem), &output_buffer_c);
  
  const size_t global_sz_szt = (size_t) sz;
  if ( clEnqueueNDRangeKernel(command_queue, kernel_dot_prod_mul,
           1, NULL, (const size_t*) &global_sz_szt, NULL, 0, NULL, NULL)
       != CL_SUCCESS ) {
    fprintf(stderr, "cannot enqueue kernel dot_prod_mul\n");
    return 1;
  }
  
  // This barrier appears only for the purpose of demonstration. We are working
  // with and in-order command queue, so that it is implied by the enqueue
  // commands.
  if ( clEnqueueBarrierWithWaitList(command_queue, 0, NULL, NULL) != CL_SUCCESS ) {
    printf("cannot enqueue barrier\n");
    return 1;
  }

  const cl_int sz_clint = (cl_int)sz;
  clSetKernelArg(kernel_reduction, 0, sizeof(cl_mem), &output_buffer_c);
  clSetKernelArg(kernel_reduction, 1, local_redsz*sizeof(float), NULL);
  clSetKernelArg(kernel_reduction, 2, sizeof(cl_int), &sz_clint);
  clSetKernelArg(kernel_reduction, 3, sizeof(cl_mem), &output_buffer_c_sum);

  size_t global_redsz_szt = (size_t) global_redsz;
  size_t local_redsz_szt = (size_t) local_redsz;
  if ( clEnqueueNDRangeKernel(command_queue,
           kernel_reduction, 1, NULL, (const size_t *) &global_redsz_szt, (const size_t *) &local_redsz_szt,
           0, NULL, NULL)
      != CL_SUCCESS) {
    fprintf(stderr, "cannot enqueue kernel reduction\n");
    return 1;
  }

  float *c_sum = malloc(nmb_redgps*sizeof(float));
  if ( clEnqueueReadBuffer(command_queue,
           output_buffer_c_sum, CL_TRUE, 0, nmb_redgps*sizeof(float), c_sum, 0, NULL, NULL)
      != CL_SUCCESS) {
    fprintf(stderr, "cannot enqueue read of buffer c\n");
    return 1;
  }

  if ( clFinish(command_queue) != CL_SUCCESS ) {
    fprintf(stderr, "cannot finish queue\n");
    return 1;
  }
  

  float c_sum_total = 0.f;
  for (size_t ix = 0; ix < nmb_redgps; ++ix)
    c_sum_total += c_sum[ix];

  printf("Dot product equals %f\n", c_sum_total);
  
  free(a);
  free(b);
  free(c_sum);

  clReleaseMemObject(input_buffer_a);
  clReleaseMemObject(input_buffer_b);
  clReleaseMemObject(output_buffer_c);
  clReleaseMemObject(output_buffer_c_sum);

  clReleaseProgram(program);
  clReleaseKernel(kernel_dot_prod_mul);
  clReleaseKernel(kernel_reduction);

  clReleaseCommandQueue(command_queue);
  clReleaseContext(context);

  return 0;
}