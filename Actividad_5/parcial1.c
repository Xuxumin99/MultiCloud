#include <stdio.h>
#include "omp.h"

double paso;
// static long num_pasos = 100;
static long num_pasos = 1E+6;
#define NUM_THREADS 20
// #define NUM_THREADS 4

double factorial(double);

void main(){
  int i, id, nthreads;
  double e, t1, t2, tiempo, sum[NUM_THREADS];
  paso=1.0/(double)num_pasos;
  omp_set_num_threads(NUM_THREADS);
  const double startTime = omp_get_wtime();
  // printf("\nfactorial value of %ld is: %f\n",num_pasos,factorial((double)num_pasos));
  printf("\nStarting parelling of euler approximation with %i with %i threads\n",num_pasos, NUM_THREADS);
  if(num_pasos > 42990 ) num_pasos = 42990;

  #pragma omp parallel
  {
    int i, id, nthrds;
    double x;
    id = omp_get_thread_num(); 
    nthrds = omp_get_num_threads();
    if(id == 0) nthreads = nthrds;
    for (i = id, sum[id] = 0.0; i<num_pasos; i=i+nthrds){
      x=(i+0.5)*paso;
      sum[id] += 1.0/factorial((double)num_pasos-(double)i);
    }
    // printf("\nAccumulated sum %f ", sum[id]);
    // printf("in thread %i", id);
  }

  for(i=0, e= 0.0; i<nthreads; i++){
    e+=sum[i];
  }

  const double endTime = omp_get_wtime();
  tiempo = t2-t1;
  printf("\ne = (%f)\n",e+1.0);
  printf("En un tiempo total de (%lf)\n", (endTime - startTime));

} 

double factorial(double N){
    // unsigned long long fact = 1;
    // printf("Given values is: %lu", N);
    if (N == 0) 
      return 1.0;
    else 
      return N * factorial(N-1);
}

