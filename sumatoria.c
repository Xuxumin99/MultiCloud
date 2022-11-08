#include <stdio.h>
#include <omp.h>
#include <math.h>
#define NUM_THREADS 4

int main()
{  omp_set_num_threads(NUM_THREADS);

  double t1, t2, tiempo;
  t1 = omp_get_wtime();
   t2= omp_get_wtime();
   tiempo = t2- t1;
    printf(" tomo (%lf) segundos\n ", tiempo);

   return (0);}
void iteracion(int num)
{
{
	printf("Enter your number: "); 
	scanf("%d",&num); 
	int sum=0; 
	for(int i=num;i>0;--i) 
	   sum+=i; 
	printf("\nSum is: %d",sum); 
	return 0; 
} 	
}
