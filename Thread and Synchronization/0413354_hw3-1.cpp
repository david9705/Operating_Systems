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

#define THREAD_NUM 4

int imgWidth, imgHeight;
int FILTER_SIZE;
int FILTER_SCALE;
int *filter_G;
sem_t sem;
int count;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputBlur_name[5] = {
	"Blur1.bmp",
	"Blur2.bmp",
	"Blur3.bmp",
	"Blur4.bmp",
	"Blur5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;

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

unsigned char GaussianFilter(int w, int h)
{
	int tmp = 0;
	int a, b;
	int ws = (int)sqrt((float)FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_G[j*ws + i] * pic_grey[b*imgWidth + a];
	};
	tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

void *thread_func(void *ptr)
{
	int *i = (int *)ptr;
	for (int j = imgHeight*(*i)/THREAD_NUM; j < imgHeight*(*i+1)/THREAD_NUM; j++) {
		for (int k = 0; k < imgWidth; k++){
	        pic_grey[j*imgWidth + k] = RGB2grey(k, j);
	    }
	}
	
	sem_wait(&sem);
	count--;
	sem_post(&sem);
	while (count != 0)
	    ;
	
	for (int j = imgHeight*(*i)/THREAD_NUM; j < imgHeight*(*i+1)/THREAD_NUM; j++) {
		for (int k = 0; k < imgWidth; k++){
	        pic_blur[j*imgWidth + k] = GaussianFilter(k, j);
	        pic_final[3 * (j*imgWidth + k) + MYRED] = pic_blur[j*imgWidth + k];
			pic_final[3 * (j*imgWidth + k) + MYGREEN] = pic_blur[j*imgWidth + k];
			pic_final[3 * (j*imgWidth + k) + MYBLUE] = pic_blur[j*imgWidth + k];
	    }
	}
	pthread_exit(NULL);
}

int main()
{	
	// read mask file
	FILE* mask;
	mask = fopen("mask_Gaussian.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	fscanf(mask, "%d", &FILTER_SCALE);

	filter_G = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_G[i]);
	fclose(mask);

	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++){
		count = THREAD_NUM;
		//declare pthread
	    pthread_t *thread = (pthread_t *)malloc(THREAD_NUM*sizeof(pthread_t));
	    
	    //initial semaphore
	    sem_init(&sem, 0, 1);
	
            // read input BMP file
	    pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
	    // allocate space for output image
	    pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
	    pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
	    pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		
	    //convert RGB image to grey image
	    int key[THREAD_NUM] = {0, 1, 2, 3};
	    for (int i = 0; i < THREAD_NUM; i++) {
	    	pthread_create(&thread[key[i]], NULL, thread_func, &key[i]);
	    }
	    for (int i = 0; i < THREAD_NUM; i++) {
	    	pthread_join(thread[i], NULL);
	    }
            sem_close(&sem);

	    // write output BMP file
	    bmpReader->WriteBMP(outputBlur_name[k], imgWidth, imgHeight, pic_final);
       
	    //free memory space
	    free(pic_in);
	    free(pic_grey);
	    free(pic_blur);
	    free(pic_final);
	    free(thread);
	}

	return 0;
}
