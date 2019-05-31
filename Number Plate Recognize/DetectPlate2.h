#pragma once

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "preprocess.h";

using namespace std;
using namespace cv;

bool checkInvalidPlate(RotatedRect &rotatedRect);
vector <Mat> PlateFinder(Mat& imgOriginal);

const float MIN_PLATE_WIDTH = 60.0;
const float MAX_PLATE_WIDTH = 800.0;
const float MIN_PLATE_HEIGHT = 40.0;
const float MAX_PLATE_HEIGHT = 550.0;
const float MIN_PLATE_RATIO = 0.3;
const float MAX_PLATE_RATIO = 0.8;