#include <stdio.h>
#include <omp.h>
#include <math.h>
FILE *fptr;
int main()
{
const int N = 500;
fptr=fopen("Euler_n_0.txt","w");
printf("Numero de pasos:%d \n", N);
fprintf(fptr, "Datos que encuentra el metodo de Euler(variable ind.\t variable dep.\t numero de thread)\n");
    double h,t[500],w[500],ab;
    double w0=0.5,a=0,b=2;
    int i;
    // omp_get_time
    fprintf(fptr, "%f\t\n", a); // inicia pragma
    h=(b-a)/N;
    w[0] = w0;
    t[0] = a;
    for(i=0;i<N;i++){
        t[i]=a+(h*i);
        w[i]=w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]);
    }
    for(i=0;i<N;i++){
        fprintf(fptr, "%f\t %f\n", t[i], w[i]);
    }
fclose(fptr);
}
