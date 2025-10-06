#include <stdio.h>
#include <omp.h>

int tid, tprivate, rprivate;
#pragma omp threadprivate(tprivate)

int main() {
  printf("Région parallèle 1\n");
  #pragma omp parallel private(tid, rprivate)
  {
    tid = omp_get_thread_num();
    tprivate = tid;
    rprivate = tid;
    printf(
      "Fil d'exécution %d: tprivate=%d rprivate=%d\n", tid, tprivate, rprivate
    );
  }

  printf("Région parallèle 2\n");
  #pragma omp parallel private(tid, rprivate)
  {
    tid = omp_get_thread_num();
    printf(
      "Fil d'exécution %d: tprivate=%d rprivate=%d\n",
      tid, tprivate, rprivate
    );
  }
  return 0;
}