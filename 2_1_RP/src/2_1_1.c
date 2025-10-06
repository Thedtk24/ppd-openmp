#include <stdio.h>
#include <omp.h>

int main(void) {
    //omp_set_num_threads(4);

    #pragma omp parallel //num_threads(4)
    printf("Bonjour\n");
    printf("le monde !\n");
    return 0;
}