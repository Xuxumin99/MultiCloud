#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *image, *outputImage, *lecturas;
    image = fopen("hollow.bmp","rb");          //Imagen original a transformar
    outputImage = fopen("img2_dd.bmp","wb");    //Imagen transformada

    unsigned char r, g, b;               //Pixel

    for(int i=0; i<54; i++) fputc(fgetc(image), outputImage);   //Copia cabecera a nueva imagen
    while(!feof(image)){                                        //Grises
       b = fgetc(image);
       g = fgetc(image);
       r = fgetc(image);       
    
    // __int8 pixel = r+0*g+b*(1/255);
    unsigned char pixel = 0.21*r+0.72*g+0.07*b;
    fputc(pixel, outputImage);
    fputc(pixel, outputImage);
    fputc(pixel, outputImage);

    // printf("\nRGB: %i\t",r,g,b);
    // printf("new pixel: %i\n",pixel);
    }

    fclose(image);
    fclose(outputImage);
    return 0;
}
