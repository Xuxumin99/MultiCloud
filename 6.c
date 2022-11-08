#include <omp.h>
#include <stdio.h>
#include <math.h>
#define NUM_THREADS 20

static long num_steps = 1000000.00;
double step;

double factorial(double x) {
	if (x <= 0.0) {
		return 1.0;
	}
	double j, fact = 1.0;
	for(j=1.0;j<=x;j++){
		fact=fact*j;
	}
	return fact;    
}

void main () {
	omp_set_num_threads(NUM_THREADS);
	int i; double t1, t2, tiempo, x, e, sum = 0.0;
	step = 1.0/(double) num_steps;
	t1 = omp_get_wtime();
	if(num_steps >= 5000){
		num_steps = 5000;
	}
	printf ("Num steps = %ld\n", num_steps);
	#pragma omp parallel for num_threads(NUM_THREADS) reduction(+:sum)
	for (i=1; i<= num_steps; i++){
		x = (i-0.5)*step;
		sum = sum + 1.0/(factorial((double) i));
	}
	t2=omp_get_wtime();
	tiempo=t2-t1;
	printf("\nSe ejecuto en (%lf) segundos\n", tiempo);
	e = sum + 1;
	printf ("Resultado es e = %f\n", e);

    sum[id]+=1.0/sqrt(1-x*x);
    sum[id]+=4.0/sqrt(1-x*x);
    sum[id]+=pow(-1.0,(double)i-1.0)*(1.0/((2.0*(double)i)-1.0));
}