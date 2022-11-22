#include <stdio.h>
#include "omp.h"
#define NUM_THREADS 3 //3, 6, 9, 12, 15, 18

int main(){
  omp_set_num_threads(NUM_THREADS);
  const double startTime = omp_get_wtime();
  #pragma omp parallel
  {
    int ID = omp_get_thread_num();
    printf("Procesador(%d) ",ID);
    printf("Multiples(%d) ",ID);
    printf("En accion(%d)\n",ID);
  }  
  const double endTime = omp_get_wtime();
  printf("En un tiempo total de (%lf)\n", (endTime - startTime));
}