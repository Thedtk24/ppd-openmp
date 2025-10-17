#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Recherche de pivot pour le tri rapide (ne pas modifier)
void partition(int * out_pivot, int * arr, int right_limit) {
  int pivot = arr[right_limit - 1];
  int idx_left = -1;
  int idx_iter, tmp;

  for(idx_iter = 0; idx_iter < right_limit - 1; idx_iter++) {
    if(arr[idx_iter] < pivot) {
      idx_left++;
      tmp = arr[idx_left];
      arr[idx_left] = arr[idx_iter];
      arr[idx_iter] = tmp;
    }
  }

  tmp = arr[idx_left + 1];
  arr[idx_left + 1] = arr[right_limit - 1];
  arr[right_limit - 1] = tmp;
  (*out_pivot) = (idx_left + 1);
}

// Implémentation de tri par insertion (ne pas modifier)
void insertion_sort(int * arr, int right_limit) {
  for(int idx = 0; idx < right_limit - 1; ++idx) {
    int idx_min = idx;
    int idx_iter;
    for(idx_iter = idx_min + 1; idx_iter < right_limit; ++idx_iter) {
      if(arr[idx_min] > arr[idx_iter]) {
        idx_min = idx_iter;
      }
    }
    int tmp = arr[idx];
    arr[idx] = arr[idx_min];
    arr[idx_min] = tmp;
  }
}

// Fonction d'affichage
void print_sample(int * data, int size, int sample_length) {
  if (size <= 2 * sample_length) {
    for(int i = 0; i < size; i++) {
      printf("%d ", data[i]);
    }
  } else {
    for(int i = 0; (i < size) && (i < sample_length); i++) {
      printf("%d ", data[i]);
    }

    printf("... ");

    for(int i = size - sample_length; i < size; i++) {
      printf("%d ", data[i]);
    }
  }

  printf("\n");
}
// Implémentation séquentielle de référence (ne pas modifier)
void sort_reference(int * in_out_data, int right_limit) {
  if (0 >= right_limit) {
    return;
  }

  if(right_limit <= 256) {
    insertion_sort(in_out_data, right_limit);
  } else {
    int pivot;
    partition(&pivot, in_out_data, right_limit);
    sort_reference(&in_out_data[0], pivot);
    sort_reference(&in_out_data[pivot + 1], right_limit - (pivot + 1));
  }
}

// Implémentation à paralléliser avec des tâches
void sort_kernel(int * in_out_data, int right_limit) {
    if(0 >= right_limit){
        return;
    }
    int pivot;

    #pragma omp task shared(right_limit)
    {
        if(right_limit <= 256){
            insertion_sort(in_out_data, right_limit);
        }
    }
    #pragma omp task shared(right_limit)
    {
        if(right_limit > 256){
            partition(&pivot, in_out_data, right_limit);
            sort_kernel(&in_out_data[0], pivot);
            sort_kernel(&in_out_data[pivot + 1], right_limit - (pivot + 1));
        }
    }
}

int main(int argc, char ** argv){
  int size = 1000000;

  if(argc > 1) {
    int a = atoi(argv[1]);
    size = a > 0 ? a : size;
  }

  int * data_ref = (int *) malloc((size_t) size * sizeof(int));
  int * data_kernel = (int *) malloc((size_t) size * sizeof(int));
  double time_reference, time_kernel;

  if(!data_ref || !data_kernel) {
    printf("Impossible d'allouer les tableaux à trier !\n");
    return 1;
  }

  // Initialisation des tableaux à trier.
  srand(time(NULL));
  for(int idx = 0; idx < size; idx++) {
    data_ref[idx] = rand();
    data_kernel[idx] = data_ref[idx];
  }

  // Exécution de l'implémentation séquentielle de référence.
  time_reference = omp_get_wtime();
  sort_reference(data_ref, size);
  time_reference = omp_get_wtime() - time_reference;
  printf("Tri séquentiel : %3.5lf s\n", time_reference);
  // Exécution de l'implémentation parallélisée.
  time_kernel = omp_get_wtime();
  sort_kernel(data_kernel, size);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Tri parallèle : %3.5lf s\n", time_kernel);

  // Affichage des extraits des tableaux triés.
  print_sample(data_ref, size, 5);
  print_sample(data_kernel, size, 5);

  // Vérification des résultats.
  for(int idx = 1; idx < size; idx++) {
    int problem = 0;
    if(data_ref[idx - 1] > data_ref[idx]) {
      printf("Mauvais résultat ! Le tableau de référence n'a pas été trié.\n");
      problem++;
    }
    if(data_kernel[idx - 1] > data_kernel[idx]) {
      printf("Mauvais résultat ! Le tableau n'a pas été trié.\n");
      problem++;
    }
    if(problem) {
      free(data_ref);
      free(data_kernel);
      return 1;
    }
  }

  printf("Ça roule !\n");

  free(data_ref);
  free(data_kernel);
  return 0;
}