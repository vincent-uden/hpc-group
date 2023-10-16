__kernel
void
diffusion_step(
    __global double *a,
    __global double *b,
    __const double c,
    __const int cols
    )
{
  int i = get_global_id(0) + 1;
  int j = get_global_id(1) + 1;

  int index = i*cols + j;

  if (index % 2 == 0) {
  double avg_neigbours = (a[index + 1] + a[index - 1] + a[index + cols] + a[index - cols]) / 4.0;
  b[index] = a[index] + c * (avg_neigbours - a[index]);
  } else {
  double avg_neigbours = (b[index + 1] + b[index - 1] + b[index + cols] + b[index - cols]) / 4.0;
  a[index] = b[index] + c * (avg_neigbours - b[index]);
  }
}
