#include <stdio.h>
#include <omp.h>
#include <math.h>
#define NUM_THREADS 900
int main(){
    double tiempo, t1, t2;
    double divisiones = 100000; //10 ^ 5
    double divisor = 1.0 / divisiones;
    double total = 0.0;
    omp_set_num_threads(NUM_THREADS);
    const double startTime = omp_get_wtime();
    t1 = omp_get_wtime();
    #pragma omp parallel
    {
        for (double i = 0.0; i < 1.0; i = i + divisor){
        double funcion = (4.0 / (1.0 + pow(i,2))) * divisor;
        total = total + funcion;
        }
    }
    const double endTime = omp_get_wtime();
    tiempo = endTime-startTime;
    printf("Tiempo total = %lf\n", (tiempo));
}
