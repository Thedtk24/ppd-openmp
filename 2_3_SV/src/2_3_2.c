#include <stdio.h>
#include <omp.h>

#define N 1024

int main(){
    double tab[N], somme = 0.0;

    for(int i = 0; i < N; i++){
        tab[i] = 1.0;
    }

    #pragma omp parallel for reduction(+:somme)
    for(int i = 0; i < N; i++){
        somme += tab[i];
    }

    printf("Somme : %.2f\n", somme);

    return 0;
}