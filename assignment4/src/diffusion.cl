__kernel
void
diffusion_step(
    __global const double *a,
    __global double *b,
    __const double c,
    __const int cols
    )
{
  int i = get_global_id(0) + 1;
  int j = get_global_id(1) + 1;
  int index = i*cols + j;
  b[index] = a[index] + c * ((a[index + 1] + a[index - 1] + a[index + cols] + a[index - cols]) / 4.0 - a[index]);
}
