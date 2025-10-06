#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define PRECISION 1.e-20
#define MAX 5
#define N 5120

void matvec_reference(double c[N], double A[N][N], double b[N]) {
  size_t i, j;

  for(i = 0; i < N; i++) {
    c[i] = 0.;
    for(j = 0; j < N; j++) {
      c[i] += A[i][j] * b[j];
    }
  }
}

void matvec_kernel(double c[N], double A[N][N], double b[N]) {
  size_t i, j;

  for(i = 0; i < N; i++) {
    c[i] = 0.;
    #pragma omp parallel for reduction(+:c[i])
    for(j = 0; j < N; j++) {
      c[i] += A[i][j] * b[j];
    }
  }
}

int main(int argc, char ** argv) {
  double * A   = malloc(N * N * sizeof(double));
  double * b   = malloc(N * sizeof(double));
  double * c   = malloc(N * sizeof(double));
  double * ref = malloc(N * sizeof(double));
  double time_reference, time_kernel;

  srand((unsigned int) time(NULL));
  for(size_t i = 0; i < N; i++)
    b[i] = (float) rand() / (float) (RAND_MAX / MAX);
  for(size_t i = 0; i < N * N; i++)
    A[i] = (float) rand() / (float) (RAND_MAX / MAX);

  time_reference = omp_get_wtime();
  matvec_reference(ref, (double (*)[N])A, b);
  time_reference = omp_get_wtime() - time_reference;
  printf("Calcul séquentiel : %3.5lf s\n", time_reference);

  time_kernel = omp_get_wtime();
  matvec_kernel(c, (double (*)[N])A, b);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Calcul parallèle : %3.5lf s\n", time_kernel);

  for (size_t i = 0; i < N; i++) {
    printf("ref[%2d] = %f, c[%d] = %f\n", i, ref[i], i, c[i]);
    if (abs(ref[i] - c[i]) > PRECISION) {
      printf("Mauvais résultats !\n");
      exit(1);
    }
  }
  printf("Ça roule !\n");

  free(A);
  free(b);
  free(c);
  free(ref);
  return 0;
}