#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <malloc.h>
#include "omp.h"

// Standard values located at the header of an BMP file
#define MAGIC_VALUE    0X4D42 
//Bit depth
#define BITS_PER_PIXEL 24
#define NUM_PLANE      1
#define COMPRESSION    0
#define BITS_PER_BYTE  8
//OpenMP var
#define NUM_THREADS 200

#pragma pack(1)

/*Section used to declare structures*/
typedef struct{
  uint16_t type;
  uint32_t size;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offset;
  uint32_t header_size;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bits;
  uint32_t compression;
  uint32_t imagesize;
  uint32_t xresolution;
  uint32_t yresolution;
  uint32_t importantcolours;
}BMP_Header;

typedef struct{
  BMP_Header header;
  unsigned int pixel_size;
  unsigned int width;
  unsigned int height;
  unsigned int bytes_per_pixel;
  unsigned char * pixel; //For future allocation in memory
}BMP_Image;

/*Section used to declare functions*/
int checkHeader(BMP_Header *);
BMP_Image* cleanUp(FILE *, BMP_Image *);
BMP_Image* BMP_open(const char *);
int BMP_save(const BMP_Image *img, const char *filename);
void BMP_destroy(BMP_Image *img);
static int RGB2Gray(unsigned char, unsigned char, unsigned char);
void BMP_gray(BMP_Image*);

/*End section*/

int checkHeader(BMP_Header *hdr){
  if((hdr -> type) != MAGIC_VALUE)           {printf("No es un bmp\n"); return 0;}
  if((hdr -> bits) != BITS_PER_PIXEL)        {printf("Revisa bit depth\n"); return 0;}
  if((hdr -> planes) != NUM_PLANE)           {printf("Array de diferente dimensiones\n"); return 0;}
  if((hdr -> compression) != COMPRESSION)    {printf("Hay compresion\n"); return 0;}
  return 1;
}

/*Funtion used as cleaner, in incorrect format of an image*/
BMP_Image * cleanUp(FILE * fptr, BMP_Image * img)
{
  if (fptr != NULL)
    {
      fclose (fptr);
    }
  if (img != NULL)
    {
      if (img -> pixel != NULL)
	{
	  free (img -> pixel);
	}
      free (img);
    }
  return NULL;
}

BMP_Image* BMP_open(const char *filename){
  FILE *fptr = NULL;
  BMP_Image *img = NULL;
  fptr = fopen(filename, "rb");
  if(fptr == NULL){printf("Archivo no existe\n"); return cleanUp(fptr,img);}
  img = malloc(sizeof(BMP_Image));
  if(img == NULL){return cleanUp(fptr,img);}
  if(fread(&(img -> header), sizeof(BMP_Header),1,fptr) != 1) {printf("Header no disponible\n"); return cleanUp(fptr,img);}
  if(checkHeader(&(img -> header)) == 0) {printf("Header fuera del estandar\n"); return cleanUp(fptr,img);}
  img -> pixel_size      = (img -> header).size - sizeof(BMP_Header);
  img -> width           = (img -> header).width;
  img -> height          = (img -> header).height;
  img -> bytes_per_pixel = (img -> header).bits/BITS_PER_BYTE;
  img -> pixel = malloc(sizeof(unsigned char) * (img -> pixel_size));
  if((img -> pixel) == NULL){printf("Imagen vacia\n"); return cleanUp(fptr,img);}
  if(fread(img->pixel, sizeof(char), img -> pixel_size,fptr) != (img -> pixel_size)){printf("Imagen con contenido irregular \n");return cleanUp(fptr,img);}
  char onebyte;
  if(fread(&onebyte,sizeof(char),1,fptr) != 0) {printf("Hay pixeles residuales\n"); return cleanUp(fptr,img);}
  fclose(fptr);
  return img;
}

int BMP_save(const BMP_Image *img, const char *filename){
  FILE *fptr = NULL;
  fptr = fopen(filename, "wb");
  if(fptr == NULL) {return 0;}//Maybe you should write the header first
  if(fwrite(&(img -> header), sizeof(BMP_Header),1,fptr) != 1) {fclose(fptr); return 0;}
  if(fwrite(img->pixel, sizeof(char), img -> pixel_size, fptr) != (img -> pixel_size)) {fclose(fptr); return 0;}
  fclose(fptr);
  return 1;
}

void BMP_destroy(BMP_Image *img){
  free (img -> pixel);
  free (img);
}

void specs(BMP_Image* img){
  printf("Image width: %i\n", img->width);
  printf("Image height: %i\n", abs(img->height));
  printf("Image BPP: %i\n",  img->bytes_per_pixel);
  printf("Image size: %i\n",  img->pixel_size);
}

static int  RGB2Gray(unsigned char red, unsigned char green, unsigned char blue){
  double gray =  0.2989*red + 0.5870*green + 0.1140*blue;
  // double gray =  0.21*red + 0.72*green + 0.07*blue;
  //0.21*r+0.72*g+0.07*b
  return (int) gray;
}

void BMP_gray(BMP_Image *img)
{
  specs(img);
  omp_set_num_threads(NUM_THREADS);
  const double startTime = omp_get_wtime();
  
  #pragma omp parallel
  {
    #pragma omp for /*No wait*/
    for (int pxl = 0; pxl < (img -> pixel_size); pxl += 3)
    {
      unsigned char gray = RGB2Gray(img -> pixel[pxl + 2],img -> pixel[pxl + 1],img -> pixel[pxl]);
      img -> pixel[pxl + 2] = gray; //Red pixel
      img -> pixel[pxl + 1] = gray; //Green pixel
      img -> pixel[pxl]     = gray; //Blue pixel
    }
  }
  if (BMP_save(img, "GrayScale.bmp") == 0)
   {
     printf("Output file invalid!\n");
     BMP_destroy(img);
  }
  // Destroy the BMP image
  BMP_destroy(img);
  const double endTime = omp_get_wtime();
  printf("En un tiempo total de (%lf)\n", (endTime - startTime));
}

void BMP_horFlip(BMP_Image *img){
  // printf("Creare la matriz de pixeles\n");
  unsigned char * output = (unsigned char *) malloc(img->pixel_size * sizeof(unsigned char));//Allocating memory for matrix
  unsigned int r = abs(img->height);
  unsigned int c = img->width*3;
  omp_set_num_threads(NUM_THREADS);
  const double startTime = omp_get_wtime();
  // printf("(%ix%i),",r,c);
  //Modificar el array
  // printf("Modificare la posicion de pixeles\n");
  #pragma omp parallel
  {
    #pragma omp for
    for(int i = 0; i < r; i++){
      for(int j = c; j>0; j--){
        output[i*c+(c-j)] = img->pixel[i*c+j];
        // printf("(%ix%i),",i,j);
      }
      // printf("\n");
    }
    //Modificar la imagen
    // printf("Sobrescribire la modificacion\n");
    #pragma omp for
    for(int i = 0; i<r; i++){
      for(int j = 0; j<c; j++){
        img->pixel[i*c+j] = output[i*c+j];
      }
    }
  }
  //Guardar la imagen
  if (BMP_save(img, "HorizontalRot.bmp") == 0)
   {
     printf("Output file invalid!\n");
     BMP_destroy(img);
     free(output);
  }
  // Destroy the BMP image
  BMP_destroy(img);
  free(output);
  const double endTime = omp_get_wtime();
  printf("En un tiempo total de (%lf)\n", (endTime - startTime));
}

void BMP_verFlip(BMP_Image *img){
  // printf("Creare la matriz de pixeles\n");
  unsigned char * output = (unsigned char *) malloc(img->pixel_size * sizeof(unsigned char));//Allocating memory for matrix
  int r = abs(img->height);
  int c = img->width*3;
  omp_set_num_threads(NUM_THREADS);
  const double startTime = omp_get_wtime();
  // printf("(%ix%i),",r,c);
  //Modificar el array
  // printf("Modificare la posicion de pixeles\n");
  #pragma omp parallel
  {
    #pragma omp for
    for(int i = r-1; i>0; i--){
      for(int j = 0; j<=c; j++){
        // if(j%c == 0) printf("(%ix%i),",i,j);
        output[(r-i)*c+j] = img->pixel[i*c+j];
      }
      // printf("\n");
    }
    //Modificar la imagen
    // printf("Sobrescribire la modificacion\n");
    #pragma omp for
    for(int i = 0; i<r; i++){
      for(int j = 0; j<c; j++){
        img->pixel[i*c+j] = output[i*c+j];
      }
    }
  }
  //Guardar la imagen
  if (BMP_save(img, "VerticalRot.bmp") == 0)
   {
     printf("Output file invalid!\n");
     BMP_destroy(img);
     free(output);
  }
  // Destroy the BMP image
  BMP_destroy(img);
  free(output);
  const double endTime = omp_get_wtime();
  printf("En un tiempo total de (%lf)\n", (endTime - startTime));
}

/*Debugging functions*/
void printArr(unsigned char * array, unsigned int size, unsigned int EPR, unsigned int BPP){
  int i, k, h;
  // printf("\nRow = %i",size/(BPP*EPR));
  // printf("\nCol = %i",BPP*EPR);
  for(i = 0, h = -1, k=-1; i < size; i++){ 
    if(i%(size/(EPR*BPP)) == 0) k++; //Get column(w) index
    if(i%(EPR*BPP) == 0) h++; //Get row(h) index
  }
  printf("%i x %i",k,h);
}

int main(){
  BMP_gray(BMP_open("700.bmp"));
  // BMP_gray(BMP_open("hollow.bmp"));
  BMP_horFlip(BMP_open("GrayScale.bmp"));
  BMP_verFlip(BMP_open("GrayScale.bmp"));
  return 0;
}
