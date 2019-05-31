#pragma once

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// global var
const cv::Size GAUSSIAN_SMOOTH_FILTER_SIZE = cv::Size(5, 5);
const int ADAPTIVE_THRESH_BLOCK_SIZE = 19;
const int ADAPTIVE_THRESH_WEIGHT = 9;

// func
void preprocess(cv::Mat& imgOriginal, cv::Mat& imgGrayscale, cv::Mat& imgThresh);
Mat maxContrast(Mat& imgGrayscale);
