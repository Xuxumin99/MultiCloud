#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
void main()
{
FILE *image, *outputImage_GrayScale, *outputImage_Flip_vertical, 
*outputImage_Flip_horizontal;
//image = fopen("gtr35-HD.bmp", "rb"); // Imagen normal mas de (700x700) a transformar
image = fopen("vango.bmp", "rb"); 
outputImage_GrayScale = fopen("img2outv.bmp", "wb"); // Imag
outputImage_Flip_vertical = fopen("vangover.bmp", "wb"); 
outputImage_Flip_horizontal = fopen("vangohor.bmp", "wb"); // Imagen 
long ancho;
long alto;
unsigned char r, g, b; // Pixels
unsigned char pixel;
omp_set_num_threads(4);
unsigned char cabecera[54];
for (int i = 0; i < 54; i++)
{
cabecera[i] = fgetc(image);
fputc(cabecera[i], outputImage_GrayScale); // Copia cabecera a nueva imagen
fputc(cabecera[i], outputImage_Flip_vertical); 
fputc(cabecera[i], outputImage_Flip_horizontal); 
}
ancho = (long)cabecera[20] * 65536 + (long)cabecera[19] * 256 + (long)cabecera[18];
alto = (long)cabecera[24] * 65536 + (long)cabecera[23] * 256 + (long)cabecera[22];
printf("largo img %li\n", alto);//NOT OK
printf("ancho img %li\n", ancho);//OK
//DIMENSIONES MANUALES
int alto_N = 2757;//650 / 2757
int ancho_N = ancho; //1200 / 4096
long dimension_vector = ancho_N * alto_N; //Cantidad total de pixeles RGB
//Se reserva memoria considerando que cada pixel es RGB y ocupa 8 bits
unsigned char* tmp_pixel_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char));
//Vector usado para guardar temporalmente el vector girado verticalmente
unsigned char* tmp_flip_V_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char));
//[0,255].
if(tmp_pixel_vector == NULL || tmp_flip_V_vector == NULL )
{
printf("Error! Memory not allocated.");
exit(0);
}
//Uso de clausulas para eliminar "Race condition"
#pragma omp parallel
{
#pragma omp for schedule(dynamic) 
for (int i = 0; i < dimension_vector; i++)
{ 
// Debe ser ejecutado solo por un solo thread
//#pragma omp region {} no fue utilizado ya que agrega mas ruido a la imagen
#pragma omp critical
b = fgetc(image);
#pragma omp critical
g = fgetc(image);
#pragma omp critical
r = fgetc(image);
//Conversion a escala de Grises
pixel = 0.21 * r + 0.72 * g + 0.07 * b;
tmp_pixel_vector [i] = pixel;
tmp_pixel_vector [i+1] = pixel;
tmp_pixel_vector [i+2] = pixel;
}
//Imagen en escala de grises sin rotacion
#pragma omp for schedule(dynamic) 
for (int i = 0; i < dimension_vector*3; i++)
{
fputc(tmp_pixel_vector[i], outputImage_GrayScale);
fputc(tmp_pixel_vector[i+1], outputImage_GrayScale);
fputc(tmp_pixel_vector[i+2], outputImage_GrayScale);
}
//Rotado horizontalmente sobre su eje de simetria
#pragma omp for schedule(dynamic) 
for (int i = dimension_vector; i > 0; i--) 
{
fputc(tmp_pixel_vector[(i)], outputImage_Flip_horizontal);
fputc(tmp_pixel_vector[(i+1)], outputImage_Flip_horizontal);
fputc(tmp_pixel_vector[(i+2)], outputImage_Flip_horizontal);
}
//Rotado verticalmente sobre su eje de simetria
#pragma omp for schedule(dynamic) collapse(2)
for (int i = 0; i < (alto_N); i++)
{
for (int j = 0; j < (ancho_N); j++)
{
tmp_flip_V_vector [j+(ancho_N*i)] = tmp_pixel_vector [(ancho_N-j)+(i*ancho_N)];
//*(i*ancho_N)
tmp_flip_V_vector [(j+(ancho_N*i))+1]= tmp_pixel_vector [((ancho_N*j)+(i*ancho_N))-1]; 
tmp_flip_V_vector [(j+(ancho_N*i))+2]= tmp_pixel_vector [((ancho_N*j)+(i*ancho_N))-2]; 
fputc(tmp_flip_V_vector[j+(ancho_N*i)], outputImage_Flip_vertical);
fputc(tmp_flip_V_vector[(j+(ancho_N*i))+1], outputImage_Flip_vertical);
fputc(tmp_flip_V_vector[(j+(ancho_N*i))+2], outputImage_Flip_vertical);
}
}
}
free(tmp_pixel_vector);
free(tmp_flip_V_vector);
fclose(image);
fclose(outputImage_GrayScale);
fclose(outputImage_Flip_horizontal);
fclose(outputImage_Flip_vertical);
}
