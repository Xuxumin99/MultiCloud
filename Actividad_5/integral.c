#include <stdio.h>

static long num_pasos = 100000;
double paso;

int main(){
  int i;
  double x, pi, sum = 0.0;
  paso = 1.0/num_pasos;
  for(i= 0; i<num_pasos; i++){
    x=(i+0.5)*paso;
    sum = sum+4.0/(1.0+x*x);
  }
  pi = paso*sum;
  printf("pi = (%lf)\n",pi);
}