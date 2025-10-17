#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define PRECISION 1.e-20
#define MAX 5
#define RUN_PARALLEL 1
#define RUN_SEQUENTIAL 2
#define RUN_BOTH 3

// Implémentation séquentielle de référence (ne pas modifier)
void matvec_reference(double * c, double ** A, double * b, size_t N) {
  size_t i, j;

  for(i = 0; i < N; i++) {
    c[i] = 0.;
    for(j = 0; j < N; j++) {
      c[i] += A[i][j] * b[j];
    }
  }
}

void matvec_kernel(double * c, double ** A, double * b, size_t N) {
  size_t i, j;
  double temp;

  #pragma omp parallel for firstprivate(temp)
  for(i = 0; i < N; i++) {
    temp = 0.;
    #pragma omp parallel for reduction(+:temp)
    for(j = 0; j < N; j++) {
      temp += A[i][j] * b[j];
    }
    c[i] = temp;
  }
}
int main(int argc, char ** argv) {
  size_t N = 5120;
  int run = RUN_BOTH;
  if(argc > 2) {
    int arg_run = atoi(argv[1]);
    if(arg_run > 0 && arg_run <= 3) {
      run = arg_run;
    }
    size_t argN = (size_t) atoll(argv[2]);
    if(argN > 0) {
      N = argN;
    }
  } else if(argc > 1) {
    int arg_run = atoi(argv[1]);
    if(arg_run > 0 && arg_run <= 3) {
      run = arg_run;
    }
  }
  double ** A  = malloc(N * sizeof(double *));
  for(size_t i = 0; i < N; i++)
    A[i] = malloc(N * sizeof(double));
  double * b   = malloc(N * sizeof(double));
  double * c   = malloc(N * sizeof(double));
  double * ref = malloc(N * sizeof(double));
  double time_reference = .0, time_kernel = .0;

  // Initialisation des tableaux 'b' et 'A' avec des valeurs aléatoires.
  srand((unsigned int) time(NULL));
  for(size_t i = 0; i < N; i++)
    b[i] = (float) rand() / (float) (RAND_MAX / MAX);
  for(size_t i = 0; i < N; i++)
    for(size_t j = 0; j < N; j++)
    A[i][j] = (float) rand() / (float) (RAND_MAX / MAX);

  // Exécution de l'implémentation séquentielle de référence.
  if(run > RUN_PARALLEL) {
    time_reference = omp_get_wtime();
    matvec_reference(ref, A, b, N);
    time_reference = omp_get_wtime() - time_reference;
  }
  // Exécution de l'implémentation parallélisée.
  time_kernel = omp_get_wtime();
  matvec_kernel(c, A, b, N);
  time_kernel = omp_get_wtime() - time_kernel;

  // Affichage et comparaison des résultats.
  if(run == RUN_BOTH) {
    for(size_t i = 0; i < N; i++) {
      if(abs(ref[i] - c[i]) > PRECISION) {
        printf("Mauvais résultats !\n");
        exit(1);
      }
    }
  }
  double time = run == RUN_SEQUENTIAL ? time_reference : time_kernel;
  int nt = run == RUN_SEQUENTIAL ? 1 : omp_get_max_threads();

  // num_threads,time_kernel,time_reference
  if(run == RUN_BOTH)
    printf("%d,%lf,%lf\n", omp_get_max_threads(), time_kernel, time_reference);
  else
    printf("%d,%lf\n", nt, time);

  for(size_t i = 0; i < N; i++)
    free(A[i]);
  free(A);
  free(b);
  free(c);
  free(ref);
  return 0;
}