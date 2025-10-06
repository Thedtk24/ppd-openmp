#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define MAX 5
#define N 10240

void enumeration_sort_reference(double tab[N]) {
  size_t i, j;
  size_t * position = malloc(N * sizeof(size_t));
  double * copy     = malloc(N * sizeof(size_t));

  
  #pragma omp parallel
  {
    #pragma omp for
    for (i = 0; i < N; i++) {
      position[i] = 0;
      copy[i] = tab[i];
    }

    #pragma omp for firstprivate(i)
    for(j = 0; j < N; j++) {
      for(i = 0; i < N; i++) {
        if((tab[j] < tab[i]) || ((tab[i] == tab[j]) && (i < j))) {
          #pragma omp atomic
          position[i]++;
        }
      }
    }

    #pragma omp for
    for (i = 0; i < N; i++) {
      tab[position[i]] = copy[i];
    }
  }

  free(position);
  free(copy);
}

void enumeration_sort_kernel(double tab[N]) {
  size_t i, j;
  size_t * position = malloc(N * sizeof(size_t));
  double * copy     = malloc(N * sizeof(size_t));

  for(i = 0; i < N; i++) {
    position[i] = 0;
    copy[i] = tab[i];
  }

  for(j = 0; j < N; j++) {
    #pragma omp parallel for
    for(i = 0; i < N; i++) {
      if((tab[j] < tab[i]) || ((tab[i] == tab[j]) && (i < j))) {
        position[i]++;
      }
    }
  }

  for(i = 0; i < N; i++) {
    tab[position[i]] = copy[i];
  }

  free(position);
  free(copy);
}

void print_sample(double tab[], size_t size, size_t sample_length) {
  if (size <= 2 * sample_length) {
    for(size_t i = 0; i < size; i++) {
      printf("%g ", tab[i]);
    }
  } else {
    for(size_t i = 0; (i < size) && (i < sample_length); i++) {
      printf("%g ", tab[i]);
    }

    printf("... ");

    for(size_t i = size - sample_length; i < size; i++) {
      printf("%g ", tab[i]);
    }
  }

  printf("\n");
}

int main(void) {
  double * a   = malloc(N * sizeof(double));
  double * ref = malloc(N * sizeof(double));
  double time_reference, time_kernel;

  srand((unsigned int) time(NULL));
  for(size_t i = 0; i < N; i++) {
    a[i] = (float) rand() / (float) (RAND_MAX / MAX);
    ref[i] = a[i];
  }

  time_reference = omp_get_wtime();
  enumeration_sort_reference(ref);
  time_reference = omp_get_wtime() - time_reference;
  printf("Tri séquentiel : %3.5lf s\n", time_reference);

  time_kernel = omp_get_wtime();
  enumeration_sort_kernel(a);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Tri parallèle : %3.5lf s\n", time_kernel);

  print_sample(ref, N, 5);
  print_sample(a, N, 5);

  for(size_t i = 0; i < N; i++) {
    if(ref[i] != a[i]) {
      printf("Mauvais résultats !\n");
      exit(1);
    }
  }

  printf("Ça roule !\n");

  free(a);
  free(ref);
  return 0;
}