#include <stdio.h>
#include <omp.h>
#define NUM_THREADS 18 //Cambiar los threads
int main(){
    double tiempo;
    omp_set_num_threads(NUM_THREADS);
    const double startTime = omp_get_wtime();
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf("Procesadores (%d)", ID);
        printf("Multiples (%d)", ID);
        printf(",en accion --- %d\n", ID);
    }
    const double endTime = omp_get_wtime();
    tiempo = endTime-startTime;
    printf("Tiempo total = %lf\n", (tiempo));
}
