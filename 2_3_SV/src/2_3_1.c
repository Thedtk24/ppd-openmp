#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
  int val;
  #pragma omp parallel private(val)
  {
    val = rand();
    sleep(1);
    printf("val : %d\n", val);
  }
  return 0;
}