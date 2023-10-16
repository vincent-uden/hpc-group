__kernel
void
diffusion_step(
    __global const float *a,
    __global float *b,
    __const float c,
    __const int cols
    )
{
  int i = get_global_id(0);
  int j = get_global_id(1);
  int index = i*cols + j;
  float sum = -a[index];
  if (j > 0) {
    sum += a[index - 1];
  }
  if (j < cols - 1) {
    sum += a[index + 1];
  }
  if (i > 0) {
    sum += a[index - cols];
  }
  if (i < cols - 1) {
    sum += a[index + cols];
  }
  sum = sum * c / 4.0 + a[index];
  b[index] = sum;
}
