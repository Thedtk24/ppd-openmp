#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#define PI "3.141592653589793238462"
#define PRECISION 1.e-10
#define MAX 5
#define N 51200000

void pi_reference(size_t nb_steps, double * pi) {
  double term;
  double sum = 0.;
  double step = 1. / (double) nb_steps;

  for(size_t i = 0; i < nb_steps; i++) {
    term = (i + 0.5) * step;
    sum += 4. / (1. + term * term);
  }

  (*pi) = step * sum;
}

void pi_kernel(size_t nb_steps, double * pi) {
  double term;
  double sum = 0.;
  double step = 1. / (double) nb_steps;

  #pragma omp parallel for private(term) reduction(+:sum)
  for(size_t i = 0; i < nb_steps; i++) {
    term = (i + 0.5) * step;
    sum += 4. / (1. + term * term);
  }

  (*pi) = step * sum;
}
int main(void) {
  double pi, pi_ref;
  double time_reference, time_kernel;

  time_reference = omp_get_wtime();
  pi_reference(N, &pi_ref);
  time_reference = omp_get_wtime() - time_reference;
  printf("Calcul séquentiel : %3.5lf s\n", time_reference);

  time_kernel = omp_get_wtime();
  pi_kernel(N, &pi);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Calcul parallèle : %3.5lf s\n", time_kernel);

  printf("Pi : %s\n", PI);
  printf("Pi (résultat du calcul de référence) : %.22g\n", pi_ref);
  printf("Pi (résultat du calcul parallèle) : %.22g\n", pi);

  if (fabs(pi_ref - pi) > PRECISION) {
    printf("Mauvais résultats !\n");
    exit(1);
  }

  printf("Ça roule !\n");

  return 0;
}