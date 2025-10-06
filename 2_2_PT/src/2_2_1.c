#include <stdio.h>
#include <omp.h>
#define N 1024

int main(){
    double val = 0.8;
    double tab1[N], tab2[N];

    for(int i = 0; i < N; i++){
        tab1[i] = 0.6 * i;
    }

    #pragma omp parallel for
    for(int j = 0; j < N; j++){
        tab2[j] = tab1[j] + val;
    }
    

    for(int k = 0; k < N; k++){
        printf("tab2[%d] = %.2f \n", k, tab2[k]);
    }
}