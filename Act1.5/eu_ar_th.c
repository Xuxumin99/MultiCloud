#include <stdio.h>
#include <omp.h>
#include <math.h>

// Define number of threads
#define NUM_THREADS 6
// Step number
#define N 2500000 // 2,500,00 steps

FILE *fptr;

void iteration(int op_type);

static double w0 = 0.5, a = 0, b = 2;
static double h, t[N], w[N], ab;

int i, id;

int main() {
    omp_set_num_threads(NUM_THREADS);

    fptr=fopen("Euler_n_0.txt","w");

    printf("\nNumero de pasos: %d \n", N);
    fprintf(fptr, "Datos que encuentra el metodo de Euler (variable ind.\t variable dep.\t numero de thread).\n");


    double t1, t2, f_time; 

 
    const double startTime = omp_get_wtime();
    t1 = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
                (void)iteration(1);
        }
    }


    t2 = omp_get_wtime();
    f_time = t2 - t1;
    for(i=0; i<N; i++)
    {
        fprintf(fptr, "%f\t %f\n", t[i], w[i]);
    }
    fclose(fptr);
    printf("tomo (%lf) segundos\n", f_time);
}


void iteration(int op_type) {
    id = omp_get_thread_num();
    printf("ID Thread [%d]\n", id);
    h = (b-a) / N;
    w[0] = w0;
    t[0] = a;
    

    for(i=0; i<N; i++) 
    {
        t[i] = a + (h*i);
        switch (op_type)
        {
            case 1: // "exp"
                w[i] = exp2((w[i-1]+h)*(1+t[i-1]*(t[i-1]-w[i-1])));
                break;
            case 2: // "pow"
                w[i] = w[i-1]+h*(1+t[i-1]*pow(t[i-1]-w[i-1], 2));
                break;
            case 3: // "sin"
                w[i] = sin(w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]));
                break;
            case 4: // "cos"
                w[i] = cos(w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]));
                break;
            case 5: // "sqrt"
                w[i] = sqrt(w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]));
                break;
            case 6: // "sinh"
                w[i] = sinh(w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]));
                break;
            case 7: // "cosh"
                w[i] = cosh(w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]));
                break;
            default:
                w[i]=w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]);
                break;
        }
    }
}