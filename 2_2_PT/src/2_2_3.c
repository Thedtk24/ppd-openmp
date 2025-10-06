#include <stdio.h>
#define N 100

int main() {
  double a[N], b[N], c[N], d[N];

  for(size_t i = 0; i < N; i++)
    a[i] = b[i] = i;

  #pragma omp parallel
  {
    #pragma omp for schedule(static) nowait
    for(size_t i = 0; i < N; i++) {
      c[i] = a[i] + b[i];
    }

    #pragma omp for schedule(static)
    for(size_t i = 0; i < N; i++) {
      d[i] = a[i] + c[i];
    }
  }

  for(size_t i = 0; i < N; i++) {
    printf("%g ", d[i]);
  }

  printf("\n");

  return 0;
}