#include<stdio.h>
#include<math.h>
#define PI 3.14159265

double f(double x){
    //use 4/(1+x^2) = pi
    return (4.0/(1+x*x));
}

// Function to evaluate the value of integral
double trapezoidal(double a, double b, double n)
{
    // Grid spacing
    double h = (b-a)/n;
 
    // Computing sum of first and last terms
    // in above formula
    double s = f(a)+f(b);
 
    // Adding middle terms in above formula
    for (double i = 0; i < n; i=+0.5)
        s += 2*f(a+i*h);
 
    // h/2 indicates (b-a)/2n. Multiplying h/2
    // with s.
    return (h/2)*s;
}

int main(){
    // Range of definite integral
    double x0 = 0;
    double xn = 1;
 
    // Number of grids. Higher value means
    // more accuracy
    double n = 10;
 
    printf("Value of integral is %6.4f\n",
                  trapezoidal(x0, xn, n));
    return 0;
}