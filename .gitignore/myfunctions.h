#pragma once
#include <stdio.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

int labeling(IplImage *image);
int largest(int* array, int length);
int check(int* array, int val, int length);
void GetLabel(IplImage *src, IplImage *dst, int l);
CvPoint bary(IplImage *image, int A);
int Area(IplImage *image);
void Imageto2D(IplImage *image, int **arr, int A);
void ComputeCov(int *arr[2], double matrix[2][2], CvPoint B, int A);
void GetType(IplImage* image, char data[218]);
double GetOrientation(CvMat* evec, CvMat* eval, CvPoint bary);
void InverGrey(IplImage* image);
int GetWB(IplImage *image, CvPoint bary);
