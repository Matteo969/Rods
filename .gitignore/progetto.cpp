# Rods
#include "myfunctions.h"

#define FOREGROUND 0;
#define BACKGROUND 255;

int n, area, *D[2],wb;
short int treshold;
char filename[128], c[1], type[128];
double a, theta, cov[2][2];
CvPoint barycentre;
CvSeq *contours = 0;

int main(int argc, char** argv) {

	c[0] = 'b';
	CvMat* evec = cvCreateMat(2, 2, CV_64FC1);
	CvMat* eval = cvCreateMat(1, 2, CV_64FC1);
	CvMat* rot_mat = cvCreateMat(2, 3, CV_32FC1);
	CvRect rect;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvPoint2D32f p3;

	while (strcmp(c, "a") != 0) {
		IplImage *image = 0, *image2 = 0, *imagecont;

		printf("Insert the file name (q to exit): ");
		scanf("%s", filename);
		if (strcmp(filename, "q") == 0)
			c[0] = 'a';
		else {
			//file reading
			while (image == 0) {
				image = cvLoadImage(filename, -1);
				if (image == 0) {
					printf("file %s was not found\n", filename);
					printf("Insert the file name (q to exit): ");
					scanf("%s", filename);
					if (strcmp(filename, "q") == 0)
						return -1;
				}
				else {
					if (image->nChannels != 1) {
						cvReleaseImage(&image);
						printf("Image in file %s has nChannels !=1\n", filename);
						printf("Insert the file name (q to exit): ");
						scanf("%s", filename);
						if (strcmp(filename, "q") == 0)
							return -1;
					}
				}
			}
			image2 = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
			imagecont = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
			cvSmooth(image, image, CV_MEDIAN, 5, 0, 0, 0);
			cvSmooth(image, image, CV_MEDIAN, 5, 0, 0, 0);
			cvSmooth(image, image, CV_MEDIAN, 5, 0, 0, 0);
			cvSmooth(image, image, CV_MEDIAN, 5, 0, 0, 0);
			cvSmooth(image, image, CV_MEDIAN, 5, 0, 0, 0);
			cvAdaptiveThreshold(image, image, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 61, 17);
			n = labeling(image);
			
			for (int i = 0; i <= n; i++) {
				GetLabel(image, image2, i);
				area = Area(image2);
				if (area > 1500) {
					GetType(image2,type);
					barycentre = bary(image2, area);
					D[0] = (int*)malloc(area * sizeof(int));
					D[1] = (int*)malloc(area * sizeof(int));
					//transform the image into a 2D vector to put it in the next function
					Imageto2D(image2, D, area);
					//computes the covariance matrix
					ComputeCov(D, cov, barycentre, area);
					//create a CvMat covariance matrix to put inside the next function
					CvMat cov1 = cvMat(2, 2, CV_64FC1, cov); 
					//finds eigenvalues and eigenvectors of the covariance matrix
					cvEigenVV(&cov1, evec, eval, DBL_EPSILON, -1, -1);
					//computes the orientation
					theta = GetOrientation(evec, eval, barycentre);
					if (theta > 0) theta = 180 - theta;
					else if (theta <= 0) theta = -theta;
					//copies the image into a new one that will be manipulated to extract others informations
					cvCopyImage(image2, imagecont);
					//It rotates the image with an angle equal to theta to make it vertical, then finds the MER
					p3 = cvPoint2D32f(barycentre.x, barycentre.y);
					cv2DRotationMatrix(p3, 90 - theta, 1, rot_mat);//computation of the rotation matrix
					cvWarpAffine(imagecont, imagecont, rot_mat, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(255));//rotation of the image
					InverGrey(imagecont);
					wb = GetWB(imagecont, barycentre);
					cvFindContours(imagecont, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
					rect = cvBoundingRect(contours, 0);
					
					printf("%s\n", type);
					printf("area : %d\n", area);
					printf("position: ( %d %d)\n", barycentre.x, barycentre.y);
					printf("orientation: %f \n", theta);
					printf("heigth: %d \n", rect.height);
					printf("width: %d \n", rect.width);
					printf("width at the barycentre: %d \n", wb);
					cvShowImage("labeled image", image2);
					cvWaitKey(0);
				}
			}
		}
	}
	fflush(stdin);
}
