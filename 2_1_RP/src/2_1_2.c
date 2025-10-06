#include <stdio.h>
#include <omp.h>

int main(int argc, char ** argv) {
  int parallel = 1;

  if(argc > 1 && argv[1][0] == '0') {
    parallel = 0;
  }

  omp_set_num_threads(8);

  #pragma omp parallel if(parallel)
  printf("Bonjour\n");
  printf("le monde !\n");

  return 0;
}