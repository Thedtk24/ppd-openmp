#include <stdio.h>

int main() {
  int fin = 0;

  #pragma omp parallel sections
  {
    #pragma omp section 
    {
      while(!fin) {
        printf("Pas fini\n");
      }
    }
    #pragma omp section
    {
        fin = 1;
        printf("Fini\n");
    }

    
  }
  return 0;
}