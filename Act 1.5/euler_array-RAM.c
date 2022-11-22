#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <malloc.h>

FILE *fptr;

double** tab1;
double** tab2;
double** tab3;
double** tab4;
double** tab5;
double** tab6;

#define NUM_THREADS 6
double** euler_method(int);
void writeArr(double**,FILE*);
size_t malloc_usable_size(void *ptr);


int main(){
  omp_set_num_threads(NUM_THREADS);
  const double startTime = omp_get_wtime();
  #pragma omp parallel
  {
      #pragma omp sections
    {
      #pragma omp section
      tab1 = euler_method(185529);
      #pragma omp section
      tab2 = euler_method(183674);
      #pragma omp section
      tab3 = euler_method(164190);
      #pragma omp section
      tab4 = euler_method(121522);
      #pragma omp section
      tab5 = euler_method(119667);
      #pragma omp section
      tab6 = euler_method(225418);
    }
  }
  const double endTime = omp_get_wtime();
  printf("En un tiempo total de (%lf)\n", (endTime - startTime));
  
  fptr=fopen("Euler_n_0.txt","w");
  writeArr(tab1,fptr);
  fclose(fptr);
  
  fptr=fopen("Euler_n_1.txt","w");
  writeArr(tab2,fptr);
  fclose(fptr);
  
  fptr=fopen("Euler_n_2.txt","w");
  writeArr(tab3,fptr);
  fclose(fptr);
  
  fptr=fopen("Euler_n_3.txt","w");
  writeArr(tab4,fptr);
  fclose(fptr);
  
  fptr=fopen("Euler_n_4.txt","w");
  writeArr(tab5,fptr);
  fclose(fptr);
  
  fptr=fopen("Euler_n_5.txt","w");
  writeArr(tab6,fptr);
  fclose(fptr);

}

double** euler_method(int N){
  printf("Numero de pasos:%d Atendido por thread:%d\n", N,omp_get_thread_num());
  double w0=0.5,a=0,b=2;
  int i;

  double** data = malloc(sizeof(double*) * 2);
  double* w = malloc(sizeof(double) * N);
  double* t = malloc(sizeof(double) * N);
  double h,ab;

  h=(b-a)/N;
  w[0] = w0;
  t[0] = a;
  
  // data[0] =  calloc(1,sizeof(double));
  // data[1] =  calloc(1,sizeof(double));

  for(i=0;i<N;i++){
    t[i]=a+(h*i);
    // printf("%lf\t", t[i]);
    w[i]=w[i-1]+h*1+t[i-1]*t[i-1]-w[i-1];
    // printf("%lf\n", w[i]);
  }
  // printf("\n");
  *(data) = t; *(data+1) = w;
  // printArr(data);
  return data;
}

void writeArr(double** arr, FILE *fp){
    //size_t size = malloc_usable_size(*arr)/sizeof arr[0][0];
  size_t size = _msize(*arr)/sizeof arr[0][0];
    printf("%ld\n", size);
    fprintf(fp, "Datos que encuentra el metodo de Euler:\nN\t\tX\t\t\t  Y\n");
    for(int j = 0; j < size; j++){
      fprintf(fp, "%03i\t%.4lf\t%.4lf\n",j, arr[0][j], arr[1][j]);
    }
    printf("\n"); 
}