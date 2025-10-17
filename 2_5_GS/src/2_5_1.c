#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define NESTED 0

// Implémentation séquentielle de référence (ne pas modifier)
long long int fibonacci_reference(int n) {
  if(n < 2) {
    return n;
  }

  long long int x, y;

  x = fibonacci_reference(n - 1);
  y = fibonacci_reference(n - 2);

  return x + y;
}

// Implémentation à paralléliser avec des sections
long long int fibonacci_kernel_sections(int n) {
    if(n < 2) return n;
    long long int x, y;

    #pragma omp parallel sections firstprivate(n) if(n > 25)
    {
        #pragma omp section
        {
            x = fibonacci_kernel_sections(n - 1);
        }
        
        #pragma omp section
        {
            y = fibonacci_kernel_sections(n - 2);
        }
        
    }

    return x + y;
}

// Implémentation à paralléliser avec des tâches
long long int fibonacci_kernel_tasks(int n) {
  if(n < 2) {
    return n;
  }

  long long int x, y;

  #pragma omp task shared(x)
  x = fibonacci_kernel_tasks(n - 1);

  #pragma omp task shared(y)
  y = fibonacci_kernel_tasks(n - 2);

  #pragma omp taskwait

  return x + y;
}

int main(int argc, char ** argv) {
  int n = 20;

  if(argc > 1) {
    int a = atoi(argv[1]);
    n = a > 0 ? a : n;
  }

  long long int f_ref, f_tasks, f_sections;
  double time_reference, time_kernel_tasks, time_kernel_sections;

  // Exécution de l'implémentation séquentielle de référence.
  time_reference = omp_get_wtime();
  f_ref = fibonacci_reference(n);
  time_reference = omp_get_wtime() - time_reference;
  printf("Calcul séquentiel : %3.5lf s\n", time_reference);
  // Exécution des implémentations parallélisées :
  //  - version à base de sections
  omp_set_nested(NESTED);
  time_kernel_sections = omp_get_wtime();
  f_sections = fibonacci_kernel_sections(n);
  time_kernel_sections = omp_get_wtime() - time_kernel_sections;
  printf(
    "Calcul parallèle (à base de sections) : %3.5lf s\n",
    time_kernel_sections
  );
  //  - version à base de tâches
  time_kernel_tasks = omp_get_wtime();
  f_tasks = fibonacci_kernel_tasks(n);
  time_kernel_tasks = omp_get_wtime() - time_kernel_tasks;
  printf("Calcul parallèle (à base de tâches) : %3.5lf s\n", time_kernel_tasks);

  // Affichage des résultats.
  printf(
    "Fibonacci de %d (référence séquentielle)             = %lld\n",
    n, f_ref
  );
  printf(
    "Fibonacci de %d (impl. parallèle à base de sections) = %lld\n",
    n, f_sections
  );
  printf(
    "Fibonacci de %d (impl. parallèle à base de tâches)   = %lld\n",
    n, f_tasks
  );

  // Comparaison des résultats.
  if(f_sections != f_ref) {
    printf("Mauvais résultat (impl. parallèle à base de sections) !\n");
    return 1;
  }
  if(f_tasks != f_ref) {
    printf("Mauvais résultat (impl. parallèle à base de tâches) !\n");
    return 1;
  }

  printf("Ça roule !\n");

  return 0;
}