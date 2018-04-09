#include "opencv2/opencv.hpp"
#define DISPARITY_TH 10
int main()
{
	IplImage *srcImage[2];
	IplImage *grayImage[2];
	IplImage *disparity[2];
	CvSize image_size;
	CvStereoBMState *BMState;

	CvCapture* capture = cvCreateFileCapture(
		"C:/Users/hyunl/oneDrive/Study/p-Project/accident_recognize_project/2018-03-28_sample00.mp4");
	if (!capture)
		return 0;

	IplImage* frame;

	int i = 0;
	while (1)
	{		
		if (!i)
		{
			srcImage[0] = cvCloneImage(cvQueryFrame(capture));
		}
		else if (i == 1)
		{
			srcImage[1] = cvCloneImage(cvQueryFrame(capture));
			break;
		}
		else frame = cvQueryFrame(capture);
		i++;
	}
	// cvReleaseCapture(&capture);

	// srcImage[0] = cvLoadImage("tsukuba_l.png", CV_LOAD_IMAGE_GRAYSCALE);
	// srcImage[1] = cvLoadImage("tsukuba_r.png", CV_LOAD_IMAGE_GRAYSCALE);
	if (!srcImage[0] || !srcImage[1])
	{
		printf("the streo images were not found.");
		return 0;
	}

	image_size = cvGetSize(srcImage[0]);
	grayImage[0] = cvCreateImage(image_size, IPL_DEPTH_8U, 1);
	grayImage[1] = cvCreateImage(image_size, IPL_DEPTH_8U, 1);
	cvCvtColor(srcImage[0], grayImage[0], CV_BGR2GRAY);
	cvCvtColor(srcImage[1], grayImage[1], CV_BGR2GRAY);

	cvNamedWindow("grayImage[0]", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("grayImage[1]", CV_WINDOW_AUTOSIZE);
	cvShowImage("grayImage[0]", grayImage[0]);
	cvShowImage("grayImage[1]", grayImage[1]);

	// create and initialize BMState
	BMState = cvCreateStereoBMState();
	BMState->preFilterSize = 9;
	BMState->preFilterCap = 31;
	BMState->SADWindowSize = 21;
	BMState->minDisparity = -64;  // 0
	BMState->numberOfDisparities = 5 * 16;
	BMState->uniquenessRatio = 10;
	BMState->textureThreshold = 0;

	//  disparity[0] = cvCreateImage( image_size, IPL_DEPTH_16S, 1 );
	disparity[0] = cvCreateImage(image_size, IPL_DEPTH_32F, 1);
	disparity[1] = cvCreateImage(image_size, IPL_DEPTH_8U, 1);

	cvFindStereoCorrespondenceBM(
		grayImage[0], grayImage[1], disparity[0], BMState);

	cvNormalize(disparity[0], disparity[1], 0, 256, CV_MINMAX);
	cvNamedWindow("disparity", CV_WINDOW_AUTOSIZE);
	cvShowImage("disparity", disparity[1]);

	// check disparity
	int x, y;
	CvPoint p1;
	CvPoint p2;
	int d;
	for (y = 0; y < srcImage[0]->height; y += 20)
		for (x = 0; x < srcImage[0]->width; x += 10)
		{
			p1 = cvPoint(x, y);
			d = (int)cvGetReal2D(disparity[0], y, x);
			if (d < DISPARITY_TH)
				continue;
			p2.x = (int)(p1.x - d);
			p2.y = p1.y;

			cvCircle(srcImage[0], p1, 2, CV_RGB(255, 255, 255), 2);
			cvCircle(srcImage[1], p2, 2, CV_RGB(255, 255, 255), 2);

			cvShowImage("srcImage[0]", srcImage[0]);
			cvShowImage("srcImage[1]", srcImage[1]);
			//		cvWaitKey(0);
		}
	cvWaitKey(0);

	cvReleaseCapture(&capture);
	cvDestroyWindow("disparity");
	cvReleaseStereoBMState(&BMState);
	for (int i = 0; i < 2; i++)
	{
		cvReleaseImage(&disparity[i]);
		cvReleaseImage(&srcImage[i]);
	}
	return 0;
}