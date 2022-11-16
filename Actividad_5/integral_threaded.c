#include <stdio.h>
#include "omp.h"
#include <math.h> 

static long num_pasos = 1E+6;
double paso;
#define NUM_THREADS 6

void main(){
  int i, id, nthreads;
  double pi, sum[NUM_THREADS], t1, t2, tiempo;
  paso=1.0/num_pasos;
  omp_set_num_threads(NUM_THREADS);
  const double startTime = omp_get_wtime();
  #pragma omp parallel
  {
    int i, id, nthrds;
    double x;
    id = omp_get_thread_num(); 
    nthrds = omp_get_num_threads();
    if(id == 0) nthreads = nthrds;
    for (i = id, sum[id] = 0.0; i<num_pasos; i=i+nthrds){
      x=(i+0.5)*paso;
      // sum[id]+=1.0/sqrt(1-x*x);
      sum[id]+=4.0/(1.0+x*x);
      // sum[id]+=pow(-1.0, (double)i-1.0)*(1.0/((2.0*(double)i)-1.0));
    }
    // printf("\nAccumulated sum %f ", sum[id]);
    // printf("in thread %i", id);
  }

  for(i=0, pi= 0.0; i<nthreads; i++){
    pi+=sum[i]*paso;
  }

  const double endTime = omp_get_wtime();
  tiempo = t2-t1;
  printf("\nPI approximation = (%0.10f) with %ld steps and %i threads\n",pi,num_pasos,NUM_THREADS);
  printf("En un tiempo total de (%lf)\n", (endTime - startTime));

} 