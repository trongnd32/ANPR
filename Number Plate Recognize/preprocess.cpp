#include "preprocess.h"

void preprocess(Mat &imgOrigin, Mat &imgGrayscale, Mat &imgThresh) {

	cvtColor(imgOrigin, imgGrayscale, CV_RGB2GRAY);

	//imshow("gray 1", imgGrayscale);
	//imshow("origin", imgOrigin);

	Mat imgGrayscaleClone = imgGrayscale.clone();

	//equalizeHist(imgGrayscaleClone, imgGrayscale);

	//imshow("gray 2", imgGrayscale);

	Mat imgMaxContrast = maxContrast(imgGrayscale);

	Mat imgBlurred;
	GaussianBlur(imgMaxContrast, imgBlurred, GAUSSIAN_SMOOTH_FILTER_SIZE, 0);          // gaussian blur
	//imshow("imgBlurred", imgBlurred);

	//threshold(imgMaxContrast, imgThresh, 127.0, 255.0,THRESH_BINARY_INV);
	adaptiveThreshold(imgBlurred, imgThresh, 255.0, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, ADAPTIVE_THRESH_BLOCK_SIZE, ADAPTIVE_THRESH_WEIGHT);
	//cv::adaptiveThreshold(imgMaxContrast, imgThresh, 255.0, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, ADAPTIVE_THRESH_BLOCK_SIZE, ADAPTIVE_THRESH_WEIGHT);

	Mat imgThreshClone = imgThresh.clone();
	Mat M = Mat::ones(5, 5, CV_8U);
	//morphologyEx(imgThreshClone, imgThresh, MORPH_CLOSE, M);
	//morphologyEx(imgThresh, imgThresh, MORPH_OPEN, M);
}

Mat maxContrast(Mat& imgGrayscale) {
	Mat imgTopHat;
	Mat imgBlackHat;
	Mat imgIncreaseContrast1;
	Mat imgIncreaseContrast2;

	Mat strucutringElement = getStructuringElement(CV_SHAPE_RECT, Size(3,3));
	morphologyEx(imgGrayscale, imgTopHat, CV_MOP_TOPHAT, strucutringElement);
	morphologyEx(imgGrayscale, imgBlackHat, CV_MOP_BLACKHAT, strucutringElement);

	imgIncreaseContrast1 = imgGrayscale + imgTopHat;
	imgIncreaseContrast2 = imgIncreaseContrast1 - imgBlackHat;

	return(imgIncreaseContrast2);
}

