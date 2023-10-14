__kernel
void
diffusion_step(
    __global const float *a,
    __global float *b,
    __const float c,
    __const int cols
    )
{
    int i = get_global_id(0) + 1;
    int j = get_global_id(1) + 1;

    int index = i*cols + j;

    float avg_neigbours = (a[index + 1] + a[index - 1] + a[index + cols] + a[index - cols]) / 4;
    b[index] = a[index] + c * (avg_neigbours - a[index]);
}

__kernel
void
diff_abs(
    __global const float *a,
    __global float *b,
    __const float avg,
    __const int cols
  )
{
    int i = get_global_id(0) + 1;
    int j = get_global_id(1) + 1;

    int index = i*cols + j;

    b[index] = fabs(a[index] - avg);
}

__kernel
void
reduction(
  __global const float *c,
  __local float *scratch,
  __const int sz,
  __global float *result
  )
{
  int gsz = get_global_size(0);
  int gix = get_global_id(0);
  int lsz = get_local_size(0);
  int lix = get_local_id(0);

  float acc = 0;
  for ( int cix = gix; cix < sz; cix += gsz )
    acc += c[cix];

  scratch[lix] = acc;
  barrier(CLK_LOCAL_MEM_FENCE);

  for(int offset = lsz/2; offset > 0; offset /= 2) {
    if ( lix < offset )
      scratch[lix] += scratch[lix+offset];
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  if ( lix == 0 )
    result[get_group_id(0)] = scratch[0];
}
