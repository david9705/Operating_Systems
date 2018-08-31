// Student ID:
// Name      :
// Date      : 2017.11.03

#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0
#define THREAD_NUM 8

int imgWidth, imgHeight;
int FILTER_SIZE;
int *filter_X, *filter_Y;
int count;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_sobelx, *pic_sobely, *pic_final;

const char *outputSobel_name[5] = {
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};

unsigned char Sobelx(int w, int h)
{
	int sumX = 0;
	int ws = (int)sqrt((float)FILTER_SIZE);
	
	if (h == 0 || h == imgHeight - 1)
	    sumX = 0;
	else if (w == 0 || w == imgWidth - 1)
	    sumX = 0;
	else {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				sumX += filter_X[j*ws + i] * pic_grey[(h+j-1)*imgWidth + w+i-1];
			}
		}
		
		if(sumX > 255)
		    sumX = 255;
		if(sumX < 0)
		    sumX = 0;
	}
	return (unsigned char)sumX;
}

unsigned char Sobely(int w, int h)
{
	int sumY = 0;
	int ws = (int)sqrt((float)FILTER_SIZE);
	
	if (h == 0 || h == imgHeight - 1)
	    sumY = 0;
	else if (w == 0 || w == imgWidth - 1)
	    sumY = 0;
	else {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				sumY += filter_Y[j*ws + i] * pic_grey[(h+j-1)*imgWidth + w+i-1];
			}
		}
		
		if(sumY > 255)
		    sumY = 255;
		if(sumY < 0)
		    sumY = 0;
	}
	
	return (unsigned char)sumY;
}

unsigned char RGB2grey(int w, int h)
{
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

void *thread_func1(void *ptr)
{
	int *i = (int *)ptr;
	for (int j = imgHeight*(*i)*2/THREAD_NUM; j < imgHeight*(*i+1)*2/THREAD_NUM; j++) {
		for (int k = 0; k < imgWidth; k++){
	        pic_grey[j*imgWidth + k] = RGB2grey(k, j);
	    }
	}
	pthread_mutex_lock(&mutex);
	count--;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *thread_funcx(void *ptr)
{
	int *i = (int *)ptr;
	while (count != 0)
	    ;
	for (int j = imgHeight*(*i)/(THREAD_NUM/4); j < imgHeight*(*i+1)/(THREAD_NUM/4); j++) {
		for (int k = 0; k < imgWidth; k++){
	        pic_sobelx[j*imgWidth + k] = Sobelx(k, j);
	    }
	}
	pthread_exit(NULL);
}

void *thread_funcy(void *ptr)
{
	int *i = (int *)ptr;
	while (count != 0)
	    ;
	for (int j = imgHeight*(*i)/(THREAD_NUM/4); j < imgHeight*(*i+1)/(THREAD_NUM/4); j++) {
		for (int k = 0; k < imgWidth; k++){
	        pic_sobely[j*imgWidth + k] = Sobely(k, j);
	    }
	}
	pthread_exit(NULL);
}

int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);

	filter_X = new int[FILTER_SIZE];
	filter_Y = new int[FILTER_SIZE];
	for (int i = 0; i < FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_X[i]);
		
	for (int i = 0; i < FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_Y[i]);
	
	fclose(mask);


	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++){
		count = THREAD_NUM/2;
		//declare pthread
	    pthread_t *thread = (pthread_t *)malloc(THREAD_NUM*sizeof(pthread_t));
	    
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_sobelx = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_sobely = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		
		int key[THREAD_NUM] = {0, 1, 2, 3, 0, 1, 0, 1};
		for (int i = 0; i < THREAD_NUM/2; i++) {
			pthread_create(&thread[i], NULL, thread_func1, &key[i]);
		}
		
		for (int i = THREAD_NUM/2; i < THREAD_NUM*3/4; i++) {
			pthread_create(&thread[i], NULL, thread_funcx, &key[i]);
		}
		
		for (int i = THREAD_NUM*3/4; i < THREAD_NUM; i++) {
			pthread_create(&thread[i], NULL, thread_funcy, &key[i]);
		}
		
		for (int i = 0; i < THREAD_NUM; i++) {
			pthread_join(thread[i], NULL);
		}
		//extend the size form WxHx1 to WxHx3
		int tmp;
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				tmp = sqrt((double)pic_sobelx[j*imgWidth + i]*pic_sobelx[j*imgWidth + i] + (double)pic_sobely[j*imgWidth + i]*pic_sobely[j*imgWidth + i]);
				if (tmp > 255)
				    tmp = 255;
				else if (tmp < 0)
				    tmp = 0;
				pic_final[3 * (j*imgWidth + i) + MYRED] = tmp;
				pic_final[3 * (j*imgWidth + i) + MYGREEN] = tmp;
				pic_final[3 * (j*imgWidth + i) + MYBLUE] = tmp;
			}
		}

		// write output BMP file
		bmpReader->WriteBMP(outputSobel_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_sobelx);
		free(pic_sobely);
		free(pic_final);
		free(thread);
	}

	return 0;
}
