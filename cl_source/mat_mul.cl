__kernel void mat_mul(const int N, __global float *A, __global float *B,
                      __global float *C) {
  int k;
  int i = get_global_id(0);
  int j = get_global_id(1);
  float sum = 0.0f;
  for (k = 0; k < N; k++) {
    sum += A[i * N + k] * B[k * N + j];
  }
  C[i * N + j] = sum;

  // int j, k;
  // int i = get_global_id(0);

  // float awrk[1024];

  // for (k = 0; k < N; k++) {
  //   awrk[k] = A[i * N + k];
  // }

  // for (j = 0; j < N; j++) {

  //   float sum = 0.0f;
  //   for (k = 0; k < N; k++) {
  //     sum += awrk[k] * B[k * N + j];
  //   }
  //   C[i * N + j] = sum;
  // }
}