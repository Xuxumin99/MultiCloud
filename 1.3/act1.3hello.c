#include <stdio.h> 
#include "omp.h"
#define NUM_THREADS 3

int main(){
    omp_set_num_threads(NUM_THREADS);

#pragma omp parallel 
{
    
    int ID = omp_get_thread_num();
    printf("Procesadores(%d)", ID);
    printf("Multiples(%d)", ID);
    printf("en acción(%d) --- \n", ID);

}

}