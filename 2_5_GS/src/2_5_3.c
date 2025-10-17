#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define PRECISION 1e-16

// Fonction de création de matrices creuses (ne pas modifier)
double ** genmat(double ** matrix, size_t matrix_size, size_t submatrix_size){
  int null_entry, init_val = 1325;
  double * p;

  /* generating the structure */
  for (size_t ii = 0; ii < matrix_size; ii++) {
    for (size_t jj = 0; jj < matrix_size; jj++) {
      /* computing null entries */
      null_entry = 0;
      if ((ii < jj) && (ii % 3 != 0)) null_entry = 1;
      if ((ii > jj) && (jj % 3 != 0)) null_entry = 1;
      if (ii % 2 == 1) null_entry = 1;
      if (jj % 2 == 1) null_entry = 1;
      if (ii == jj) null_entry = 0;
      if (ii == jj - 1) null_entry = 0;
      if (ii - 1 == jj) null_entry = 0; 

      /* allocating matrix */
      if (!null_entry) {
        matrix[ii * matrix_size + jj] =
          (double *) malloc(
            submatrix_size * submatrix_size * sizeof(double)
          );

        if (matrix[ii * matrix_size + jj] == NULL) return NULL;

        /* initializing matrix */
        p = matrix[ii * matrix_size + jj];
        for (size_t i = 0; i < submatrix_size; i++) {
          for (size_t j = 0; j < submatrix_size; j++) {
            init_val = (3125 * init_val) % 65536;
            (*p) = (double) ((init_val - 32768.0) / 16384.0);
            p++;
          }
        }
      } else {
        matrix[ii * matrix_size + jj] = NULL;
      }
    }
  }
  return matrix;
}

// Fonction d'allocation de blocs pleins (ne pas modifier)
double * allocate_clean_block(size_t submatrix_size) {
  double * p, * q;
  p = (double *) malloc(submatrix_size * submatrix_size * sizeof(double));
  q = p;

  if (p != NULL) {
    for (size_t i = 0; i < submatrix_size; i++) {
      for (size_t j = 0; j < submatrix_size; j++) {
        (*p) = 0.0;
        p++;
      }
    }
  } else {
    fprintf(stderr, "Error: Failed to allocate memory for a block.\n");
    return NULL;
  }

  return q;
}

// Fonction de copie de matrice
double ** matcpy(double ** __dest, double ** __src, size_t m, size_t n) {
  for(size_t i = 0; i < m * m; i++) {
    if(__src[i]) {
      __dest[i] = allocate_clean_block(n);
      for(size_t j = 0; j < n * n; j++) {
        __dest[i][j] = __src[i][j];
      }
    } else {
      __dest[i] = NULL;
    }
  }

  return __dest;
}

// Fonction de comparaison de matrices
int matchk(double ** __m1, double ** __m2, size_t m, size_t n) {
  for(size_t i = 0; i < m * m; i++) {
    if(__m1[i] && __m2[i]) {
      for(size_t j = 0; j < n * n; j++) {
        if(abs(__m1[i][j] - __m2[i][j]) > PRECISION) {
          return 1;
        }
      }
    } else if((!__m1[i] && __m2[i]) || (__m1[i] && !__m2[i])) {
      return 1;
    }
  }
  return 0;
}

// Fonction de déallocation de matrices
void matdel(double ** __m, size_t m) {
  for(size_t i = 0; i < m * m; i++) {
    if(__m[i]) {
      free(__m[i]);
    }
  }
  free(__m);
}

// Fonctions de calcul intermédiaires (ne pas modifier)
void lu0(double * diag, size_t submatrix_size){
  for (size_t k = 0; k < submatrix_size; k++) {
    for (size_t i = k + 1; i < submatrix_size; i++) {
      diag[i * submatrix_size + k] = 
        diag[i * submatrix_size + k] / diag[k * submatrix_size + k];
      for (size_t j = k + 1; j < submatrix_size; j++) {
        diag[i * submatrix_size + j] =
          diag[i * submatrix_size + j] - diag[i * submatrix_size + k] *
          diag[k * submatrix_size + j];
      }
    }
  }
}
void bdiv(double * diag, double * row, size_t submatrix_size) {
  for (size_t i = 0; i < submatrix_size; i++) {
    for (size_t k = 0; k < submatrix_size; k++) {
      row[i * submatrix_size + k] =
        row[i * submatrix_size + k] / diag[k * submatrix_size + k];
      for (size_t j = k + 1; j < submatrix_size; j++) {
        row[i * submatrix_size + j] =
          row[i * submatrix_size + j] - row[i * submatrix_size + k] *
          diag[k * submatrix_size + j];
      }
    }
  }
}
void bmod(double * row, double * col, double * inner, size_t submatrix_size){
  for (size_t i = 0; i < submatrix_size; i++) {
    for (size_t j = 0; j < submatrix_size; j++) {
      for (size_t k = 0; k < submatrix_size; k++) {
        inner[i * submatrix_size + j] =
          inner[i * submatrix_size + j] - row[i * submatrix_size + k] *
          col[k * submatrix_size + j];
      }
    }
  }
}
void fwd(double * diag, double * col, size_t submatrix_size) {
  for (size_t j = 0; j < submatrix_size; j++) {
    for (size_t k = 0; k < submatrix_size; k++) { 
      for (size_t i = k + 1; i < submatrix_size; i++) {
        col[i * submatrix_size + j] =
          col[i * submatrix_size + j] - diag[i * submatrix_size + k] *
          col[k * submatrix_size + j];
      }
    }
  }
}
// Implémentation séquentielle de référence (ne pas modifier)
int sparselu_reference(
  double ** matrix, size_t matrix_size, size_t submatrix_size
) {
  for (size_t kk = 0; kk < matrix_size; kk++) {
    lu0(matrix[kk * matrix_size + kk], submatrix_size);
    for (size_t jj = kk + 1; jj < matrix_size; jj++) {
      if (matrix[kk * matrix_size + jj] != NULL) {
        fwd(
          matrix[kk * matrix_size + kk], matrix[kk * matrix_size + jj],
          submatrix_size
        );
      }
    }
    for (size_t ii = kk + 1; ii < matrix_size; ii++) {
      if (matrix[ii * matrix_size + kk] != NULL) {
        bdiv(
          matrix[kk * matrix_size + kk], matrix[ii * matrix_size + kk],
          submatrix_size
        );
      }
    }
    for (size_t ii = kk + 1; ii < matrix_size; ii++) {
      if (matrix[ii * matrix_size + kk] != NULL) {
        for (size_t jj = kk + 1; jj < matrix_size; jj++) {
          if (matrix[kk * matrix_size + jj] != NULL) {
            matrix[ii * matrix_size + jj] = 
              !matrix[ii * matrix_size + jj] ? 
                allocate_clean_block(submatrix_size) : 
                  matrix[ii * matrix_size + jj];
            bmod(
              matrix[ii * matrix_size + kk], matrix[kk * matrix_size + jj],
              matrix[ii * matrix_size + jj], submatrix_size
            );
          }
        }
      }
    }
  }
  return 0;
}

// Implémentation à paralléliser avec des tâches
int sparselu_kernel(
  double ** matrix, size_t matrix_size, size_t submatrix_size
) {
  for (size_t kk = 0; kk < matrix_size; kk++) {
    lu0(matrix[kk * matrix_size + kk], submatrix_size);
    for (size_t jj = kk + 1; jj < matrix_size; jj++) {
      if (matrix[kk * matrix_size + jj] != NULL) {
        fwd(
          matrix[kk * matrix_size + kk], matrix[kk * matrix_size + jj],
          submatrix_size
        );
      }
    }
    for (size_t ii = kk + 1; ii < matrix_size; ii++) {
      if (matrix[ii * matrix_size + kk] != NULL){
        bdiv(
          matrix[kk * matrix_size + kk], matrix[ii * matrix_size + kk],
          submatrix_size
        );
      }
    }
    for (size_t ii = kk + 1; ii < matrix_size; ii++) {
      if (matrix[ii * matrix_size + kk] != NULL) {
        for (size_t jj = kk + 1; jj < matrix_size; jj++) {
          if (matrix[kk * matrix_size + jj] != NULL) {
            matrix[ii * matrix_size + jj] = 
              !matrix[ii * matrix_size + jj] ? 
                allocate_clean_block(submatrix_size) : 
                  matrix[ii * matrix_size + jj];
            bmod(
              matrix[ii * matrix_size + kk], matrix[kk * matrix_size + jj],
              matrix[ii * matrix_size + jj], submatrix_size
            );
          }
        }
      }
    }
  }
  return 0;
}
int main(int argc, char ** argv) {
  size_t matrix_size = 25, submatrix_size = 25;

  if(argc > 2) {
    size_t a;
    a = strtoul(argv[1], NULL, 0);
    if(a > 0) {
      matrix_size = a;
    }
    a = strtoul(argv[2], NULL, 0);
    if(a > 0) {
      submatrix_size = a;
    }
  }

  double ** matrix_ref, ** matrix_kernel;
  matrix_ref =
    (double **) malloc(matrix_size * matrix_size * sizeof(double *));
  matrix_kernel =
    (double **) malloc(matrix_size * matrix_size * sizeof(double *));

  if(!matrix_ref || !matrix_kernel) {
    printf("Impossible d'allouer les matrices à factoriser !\n");
    return 1;
  }

  matrix_ref = genmat(matrix_ref, matrix_size, submatrix_size);
  matrix_kernel =
    matcpy(matrix_kernel, matrix_ref, matrix_size, submatrix_size);

  double time_reference, time_kernel;
  int r = 0;

  // Exécution de l'implémentation séquentielle de référence.
  time_reference = omp_get_wtime();
  r = sparselu_reference(matrix_ref, matrix_size, submatrix_size);
  time_reference = omp_get_wtime() - time_reference;
  if(r) {
    printf("Erreur lors de l'exécution de l'implémentation de référence !\n");
  }
  printf("Factorisation séquentielle : %3.5lf s\n", time_reference);
  // Exécution de l'implémentation parallélisée.
  time_kernel = omp_get_wtime();
  r = sparselu_kernel(matrix_kernel, matrix_size, submatrix_size);
  if(r) {
    printf("Erreur lors de l'exécution de l'implémentation parallélisée !\n");
  }
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Factorisation parallèle : %3.5lf s\n", time_kernel);

  if(matchk(matrix_ref, matrix_kernel, matrix_size, submatrix_size)) {
    printf("Mauvais résultat !\n");
    matdel(matrix_ref, matrix_size);
    matdel(matrix_kernel, matrix_size);
    return 1;
  }

  printf("Ça roule !\n");

  matdel(matrix_ref, matrix_size);
  matdel(matrix_kernel, matrix_size);

  return 0;
}