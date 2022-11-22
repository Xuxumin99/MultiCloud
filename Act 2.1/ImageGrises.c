#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void main()
{
    FILE *image, *outputImage_GrayScale, *outputImage_Flip_vertical, *outputImage_Flip_horizontal;
    image = fopen("gtr35-HD.bmp", "rb");  // Imagen normal mas de (700x700) a transformar
    //image = fopen("gtr35-4K.bmp", "rb"); // Imagen  gran formato (más de 2000 pixeles en ancho o alto) a transformar
    outputImage_GrayScale = fopen("img2_GrayScale.bmp", "wb"); // Imagen transformada a escala de grises
    outputImage_Flip_vertical = fopen("img2_Flip_vertical.bmp", "wb"); // Imagen transformada rotada verticalmente
    outputImage_Flip_horizontal = fopen("img2_Flip_horizontal.bmp", "wb"); // Imagen transformada rotada horizontalmente
    long ancho;
    long alto;
    unsigned char r, g, b; // Pixels
    unsigned char pixel;
    omp_set_num_threads(10);

    double t1, t2, tiempo1, tiempo2, tiempo3;

    unsigned char cabecera[54];
    for (int i = 0; i < 54; i++)
    {
        cabecera[i] = fgetc(image);
        fputc(cabecera[i], outputImage_GrayScale); // Copia cabecera a nueva imagen
        fputc(cabecera[i], outputImage_Flip_vertical); 
        fputc(cabecera[i], outputImage_Flip_horizontal); 
    }

    ancho = (long)cabecera[20] * 65536 + (long)cabecera[19] * 256 + (long)cabecera[18];
    alto = (abs)((long)(cabecera[25] << 24) + (long)(cabecera[24] << 16) + (long)(cabecera[23] << 8) + (long)(cabecera[22] << 0));
    printf("largo img %li\n", alto);//NOT OK
    printf("ancho img %li\n", ancho);//OK

    //DIMENSIONES MANUALES
    int alto_N = alto;//650 / 2757
    int ancho_N = ancho; //1200 / 4096
    long dimension_vector =  ancho_N * alto_N; //Cantidad total de pixeles RGB

    //Se reserva memoria considerando que cada pixel es RGB y ocupa 8 bits
    unsigned char* tmp_pixel_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char));
    //Vector usado para guardar temporalmente el vector girado verticalmente
    unsigned char* tmp_flip_V_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char)); //[0,255].
    
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

        t1 = omp_get_wtime();
        //Imagen en escala de grises sin rotacion
        #pragma omp for schedule(dynamic) 
        for (int i = 0; i < dimension_vector*3; i++)
        {
            fputc(tmp_pixel_vector[i], outputImage_GrayScale);
            fputc(tmp_pixel_vector[i+1], outputImage_GrayScale);
            fputc(tmp_pixel_vector[i+2], outputImage_GrayScale);
        }
        t2 = omp_get_wtime();
        tiempo1 = t2 - t1;


        t1 = omp_get_wtime();
        //Rotado horizontalmente sobre su eje de simetria
        #pragma omp for schedule(dynamic) 
        for (int i = dimension_vector; i > 0; i--) 
        {
            fputc(tmp_pixel_vector[(i)], outputImage_Flip_horizontal);
            fputc(tmp_pixel_vector[(i+1)], outputImage_Flip_horizontal);
            fputc(tmp_pixel_vector[(i+2)], outputImage_Flip_horizontal);
        }
        t2 = omp_get_wtime();
        tiempo2 = t2 - t1;


        t1 = omp_get_wtime();
        //Rotado verticalmente sobre su eje de simetria
        #pragma omp for schedule(dynamic) collapse(2)
        for (int i = 0; i < (alto_N); i++)
        {
            for (int j = 0; j < (ancho_N); j++)
            {
                tmp_flip_V_vector [j+(ancho_N*i)] = tmp_pixel_vector [(ancho_N-j)+(i*ancho_N)]; //*(i*ancho_N)
                tmp_flip_V_vector [(j+(ancho_N*i))+1]=  tmp_pixel_vector [((ancho_N-j)+(i*ancho_N))-1]; 
                tmp_flip_V_vector [(j+(ancho_N*i))+2]=  tmp_pixel_vector [((ancho_N-j)+(i*ancho_N))-2]; 

                fputc(tmp_flip_V_vector[j+(ancho_N*i)], outputImage_Flip_vertical);
                fputc(tmp_flip_V_vector[(j+(ancho_N*i))+1], outputImage_Flip_vertical);
                fputc(tmp_flip_V_vector[(j+(ancho_N*i))+2], outputImage_Flip_vertical);
            }
        }

        t2 = omp_get_wtime();
        tiempo3 = t2 - t1;
    }

    printf(" Imagen en escala de grises sin rotacion tomo (%lf) segundos\n ", tiempo1);
    printf(" Imagen rotada horizontalmente sobre su eje de simetria tomo (%lf) segundos\n ", tiempo2);
    printf(" Imagen rotada verticalmente sobre su eje de simetria tomo (%lf) segundos\n ", tiempo3);

    free(tmp_pixel_vector);
    free(tmp_flip_V_vector);
    fclose(image);
    fclose(outputImage_GrayScale);
    fclose(outputImage_Flip_horizontal);
    fclose(outputImage_Flip_vertical);
}

/*
//V9: 3 operaciones paralelizadas
*/