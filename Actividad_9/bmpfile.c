#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include "omp.h"

// Standard values located at the header of an BMP file
#define MAGIC_VALUE    0X4D42 
//Bit depth
#define BITS_PER_PIXEL 24
#define NUM_PLANE      1
#define COMPRESSION    0
#define BITS_PER_BYTE  8

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
  img -> height          = abs((img -> header).height);
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

void specs(BMP_Header* header){
  printf("Image type: %i\n", header->type);
  printf("Image size: %i\n", header->size);
  printf("Image offset: %i\n", header->offset);
  printf("Image header_size: %i\n", header->header_size);
  printf("Image width: %i\n",  header->width);
  printf("Image height: %i\n",  header->height);
  printf("Image planes: %i\n", header->planes);
  printf("Image bits: %i\n", header->bits);
  printf("Image xres: %i\n", header->xresolution);
  printf("Image yres: %i\n", header->yresolution);
 /*  printf("Image res1: %i\n",  header->reserved1);
  printf("Image res2: %i\n", header->reserved2);
  printf("Image compression: %i\n", header->compression);
  printf("Image imagesize: %i\n", header->imagesize);
  printf("Image importantcolors: %i\n", header->importantcolours ); */
}

static int  RGB2Gray(unsigned char red, unsigned char green, unsigned char blue){
  double gray =  0.2989*red + 0.5870*green + 0.1140*blue;
  // double gray =  0.21*red + 0.72*green + 0.07*blue;
  //0.21*r+0.72*g+0.07*b
  return (int) gray;
}

void BMP_gray(BMP_Image *img)
{
  // specs(img);
  for (int pxl = 0; pxl < (img -> pixel_size); pxl += 3)
    {
      unsigned char gray = RGB2Gray(img -> pixel[pxl + 2],img -> pixel[pxl + 1],img -> pixel[pxl]);
      img -> pixel[pxl + 2] = gray; //Red pixel
      img -> pixel[pxl + 1] = gray; //Green pixel
      img -> pixel[pxl]     = gray; //Blue pixel
    }
  if (BMP_save(img, "GrayScale.bmp") == 0)
   {
     printf("Output file invalid!\n");
     BMP_destroy(img);
  }
  // Destroy the BMP image
  BMP_destroy(img);
}

void BMP_horFlip(BMP_Image *img){
  // printf("Creare la matriz de pixeles\n");
  unsigned char * output = (unsigned char *) malloc(img->pixel_size * sizeof(unsigned char));//Allocating memory for matrix
  unsigned int r = img->height;
  unsigned int c = img->width*3;
  // printf("(%ix%i),",r,c);
  //Modificar el array
  // printf("Modificare la posicion de pixeles\n");
  for(int i = 0; i < r; i++){
    for(int j = c; j>0; j--){
      output[i*c+(c-j)] = img->pixel[i*c+j];
      // printf("(%ix%i),",i,j);
    }
    // printf("\n");
  }
   //Modificar la imagen
  // printf("Sobrescribire la modificacion\n");
  for(int i = 0; i<r; i++){
    for(int j = 0; j<c; j++){
      img->pixel[i*c+j] = output[i*c+j];
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
}

void BMP_verFlip(BMP_Image *img){
  // printf("Creare la matriz de pixeles\n");
  unsigned char * output = (unsigned char *) malloc(img->pixel_size * sizeof(unsigned char));//Allocating memory for matrix
  int r = img->height;
  int c = img->width*3;
  // printf("(%ix%i),",r,c);
  //Modificar el array
  // printf("Modificare la posicion de pixeles\n");
  for(int i = r-1; i>0; i--){
    for(int j = 0; j<=c; j++){
      // if(j%c == 0) printf("(%ix%i),",i,j);
      output[(r-i)*c+j] = img->pixel[i*c+j];
    }
    // printf("\n");
  }
   //Modificar la imagen
  // printf("Sobrescribire la modificacion\n");
  for(int i = 0; i<r; i++){
    for(int j = 0; j<c; j++){
      img->pixel[i*c+j] = output[i*c+j];
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
}

float ** kernel(unsigned int size){
  unsigned int height = size;
  unsigned int width = size*3; 
  float ** matrix  = malloc(sizeof(float*)*height);
  for(int i = 0; i < height; i++){
    matrix[i] = malloc(sizeof(float)*width);
  }
  
  for(int i = 0; i<height; i++){
    for(int j = 0; j<width; j++){
      matrix[i][j] = (float)1.0/(size*size);
    }
  }
  return matrix;
}

char ** pixelMat(BMP_Image * img){
  unsigned int height = img->height;
  unsigned int width = img->width*3;
  char** mat = malloc(sizeof(char*) * (height));
  for(int i = 0; i < height; i++){
    mat[i] = malloc(sizeof(char)*(width));
  }
  
  for(int i=0; i < height; i++){
    for(int j=0; j< width; j++){
      mat[i][j] = img->pixel[i*width+j];
    }
  }

  return mat;
}

void BMP_blur(char* open, unsigned int size){
  unsigned int pad = (size - 1)/2;
  BMP_Image * img = BMP_open(open);
  char** out_buffer = pixelMat(img);
  float** kerSn = kernel(size);

  unsigned int height = img->height;
  unsigned int width = img->width * 3;

  int M = (size-1)/2;

  const double startTime = omp_get_wtime();

	for(int x=M;x<height-M;x++)
	{
		for(int y=M;y<width-M;y++)
		{
			float sum= 0.0;
			for(int i=-M;i<=M;++i)									
			{
				for(int j=-M;j<=M;++j)
				{
					sum+=(float)kerSn[i+M][j+M]*img->pixel[(x+i)*width+(y+j)];	//matrix multiplication with kernel
				}
			}
			out_buffer[x][y]=(char)sum;
		}
	}
 
  for(int i = 1; i<height-1; i++){
    for(int j = 1; j<width-1; j++){
      img->pixel[i*width+j] = out_buffer[i][j];
    }
  }

  char* name;
  if(strcmp(open,"HorizontalRot.bmp") == 0){
    char filename[] = "Rblur0X.bmp";
    if(size < 10) filename[6]=size+'0';
    else{
      filename[5]= (size/10)+'0';
      filename[6]= (size%10)+'0';
    }
    name = filename; 
  }
  else{
    char filename[] = "Blur0X.bmp";
    if(size < 10) filename[5]=size+'0';
    else{
      filename[4]= (size/10)+'0';
      filename[5]= (size%10)+'0';
    }
    name = filename; 
  }

  //Guardar la imagen
  if (BMP_save(img, name) == 0)
   {
     printf("Output file invalid!\n");
     BMP_destroy(img);
     free(kerSn);
     free(out_buffer);
    //  free(buffer);
  }
  // Destroy the BMP image
  BMP_destroy(img);
  free(kerSn);
  free(out_buffer);
  // free(buffer);
  const double endTime = omp_get_wtime();
  printf("%s teminado en un tiempo total de (%lf)\n",name, (endTime - startTime));
}

/*Debugging functions*/
void printArr(float ** array, int size){
  for(int i = 0; i < size; i++){
    for(int j = 0; j< size*3; j++){
      if((j+1)%3 == 0) printf("%f\t", array[i][j]);
      else printf("%f,", array[i][j]);
    }
    printf("\n");
  }
}
  
int main(){
  BMP_blur("GrayScale.bmp",3);
  BMP_blur("GrayScale.bmp",5);
  BMP_blur("GrayScale.bmp",7);
  BMP_blur("GrayScale.bmp",9);
  BMP_blur("GrayScale.bmp",11);
  BMP_blur("GrayScale.bmp",13);
  BMP_blur("GrayScale.bmp",15);
  BMP_blur("GrayScale.bmp",17);
  BMP_blur("GrayScale.bmp",19);
  BMP_blur("GrayScale.bmp",21);
  BMP_blur("GrayScale.bmp",23);
  BMP_blur("HorizontalRot.bmp",23);
  BMP_blur("HorizontalRot.bmp",21);
  BMP_blur("HorizontalRot.bmp",19);
  BMP_blur("HorizontalRot.bmp",17);
  BMP_blur("HorizontalRot.bmp",15);
  BMP_blur("HorizontalRot.bmp",13);
  BMP_blur("HorizontalRot.bmp",11);
  BMP_blur("HorizontalRot.bmp",9);
  BMP_blur("HorizontalRot.bmp",7);
  BMP_blur("HorizontalRot.bmp",5);
  BMP_blur("HorizontalRot.bmp",3);
  // BMP_gray(BMP_open("opossum.bmp"));
  // BMP_Image * img = BMP_open("opossum.bmp");
  // specs(&(img->header));
  return 0;
}
