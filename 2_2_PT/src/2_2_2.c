#include <stdio.h>
#include <omp.h>
#define N  100
#define PORTION  10

int main() {
  int tid;
  double a[N], b[N], c[N];

  for(size_t i = 0; i < N; i++) {
    a[i] = b[i] = i;
  }

  #pragma omp parallel private(tid)
  {
    tid = omp_get_thread_num();
    if(tid == 0) {
      printf("Nombre de fil. exécution = %d\n", omp_get_num_threads());
    }

    printf("Fil d'exécution %d: démarrage...\n", tid);

    #pragma omp for schedule(dynamic, PORTION)
    for(size_t i = 0; i < N; i++) {
      c[i] = a[i] + b[i];
      printf("Fil d'exécution %d: c[%2zu] = %g\n", tid, i, c[i]);
    }
  }
  return 0;
}