#pragma once

#include <string>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include "preprocess.h"
#include "ExternVariable.h"
#include "DetectPlate.h"

using namespace std;
using namespace cv;

const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;

const double SAME_LINE_RATIO = 0.8;

bool cmp(vector <Point> a, vector <Point> b);
void initKnearest();
vector <vector <Point> > detectChars(Mat& imgPlate);
string recognizeChar(vector <vector <Point> >& charsOfPlate, Mat& imgThresh);
string detectChar(Mat& imgPlate);
bool checkInvalid(string plate);
