#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <omp.h>
#define PRIME_MIN  3
#define PRIME_MAX  100000

// Implémentation séquentielle de référence (ne pas modifier)
void prime_reference(size_t primes[], size_t * ptr_nb_primes) {
  size_t nb_primes = 0;
  size_t divisor;
  bool is_prime;

  for(size_t i = PRIME_MIN; i < PRIME_MAX; i += 2) {
    is_prime = true;
    divisor = PRIME_MIN;

    while((divisor < i) && is_prime) {
      if((i % divisor) == 0) {
        is_prime = false;
      }
      divisor += 2;
    }

    if(is_prime) {
      primes[nb_primes] = i;
      nb_primes++;
    }
  }

  (*ptr_nb_primes) = nb_primes;
}

// Implémentation à paralléliser
void prime_kernel(size_t primes[], size_t * ptr_nb_primes) {
  size_t nb_primes = 0;
  size_t divisor;
  bool is_prime;

  #pragma omp parallel for
  for (size_t i = PRIME_MIN; i < PRIME_MAX; i += 2) {
    is_prime = true;
    divisor = PRIME_MIN;

    while((divisor < i) && is_prime) {
      if((i % divisor) == 0) {
        is_prime = false;
      }
      divisor += 2;
    }

    if (is_prime) {
      primes[nb_primes] = i;
      nb_primes++;
    }
  }

  (*ptr_nb_primes) = nb_primes;
}

// Fonction d'affichage
void print_sample(size_t tab[], size_t size, size_t sample_length) {
  if (size <= 2 * sample_length) {
    for(size_t i = 0; i < size; i++) {
      printf("%zu ", tab[i]);
    }
  } else {
    for(size_t i = 0; (i < size) && (i < sample_length); i++) {
      printf("%zu ", tab[i]);
    }

    printf("... ");

    for(size_t i = size - sample_length; i < size; i++) {
      printf("%zu ", tab[i]);
    }
  }

  printf("\n");
}

int main() {
  size_t * primes_ref = malloc(PRIME_MAX / 2 * sizeof(size_t));
  size_t * primes     = malloc(PRIME_MAX / 2 * sizeof(size_t));
  size_t nb_primes_ref, nb_primes;
  double time_reference, time_kernel;

  // Exécution de l'implémentation séquentielle de référence.
  time_reference = omp_get_wtime();
  prime_reference(primes_ref, &nb_primes_ref);
  time_reference = omp_get_wtime() - time_reference;
  printf("Calcul séquentiel : %3.5lf s\n", time_reference);
  // Exécution de l'implémentation parallélisée.
  time_kernel = omp_get_wtime();
  prime_kernel(primes, &nb_primes);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Calcul parallèle : %3.5lf s\n", time_kernel);

  // Affichage des extraits des résultats.
  print_sample(primes_ref, nb_primes_ref, 5);
  print_sample(primes, nb_primes, 5);

  // Comparaison des résultats.
  if(nb_primes_ref != nb_primes) {
    printf(
      "Mauvais résultat ! "
      "Le nombre de nombres premiers calculés n'est pas correct.\n"
    );
    exit(1);
  }

  int faux = 0;
  for(size_t i = 0; i < nb_primes; i++) {
    if(primes_ref[i] != primes[i]) {
      faux++;
    }
  }

  if(faux) {
    printf("Mauvais résultat ! Les nombres premiers ne correspondent pas.\n");
    exit(1);
  }

  printf("Ça roule !\n");

  free(primes_ref);
  free(primes);
  return 0;
}