#pragma once

#include <string>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include "preprocess.h"
#include "ExternVariable.h"

using namespace std;
using namespace cv;

const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;

const double SAME_LINE_RATIO = 0.85;

const double PLATE_WIDTH_PADDING_FACTOR = 1.3;
const double PLATE_HEIGHT_PADDING_FACTOR = 1.5;

const int MIN_PIXEL_WIDTH = 2;
const int MIN_PIXEL_HEIGHT = 8;

const double MIN_ASPECT_RATIO = 0.2;
const double MAX_ASPECT_RATIO = 1.0;

const int MIN_PIXEL_AREA = 80;

const double MIN_DIAG_SIZE_MULTIPLE_AWAY = 0.3;
const double MAX_DIAG_SIZE_MULTIPLE_AWAY = 3.5;
const double MAX_ANGLE_BETWEEN_CHARS = 12.0;

const double MAX_CHANGE_IN_AREA = 1.3;

const double MAX_CHANGE_IN_WIDTH = 0.8;
const double MAX_CHANGE_IN_HEIGHT = 0.3;

const int MIN_NUMBER_OF_NEARLY_CHARS = 5;

const double PLATE_WIDTH_SIZE = 800.0;

const int CHAR_DISTANCE_X = 25;
const int CHAR_DISTANCE_Y = 25;

bool cmp(vector <Point> a, vector <Point> b);
void initKnearest();
string detectChars(Mat& imgPlate);
vector <vector <Point> > removeImpossibleChars(vector <vector <Point> > contours);
bool checkPossibleChar(vector <Point> pchar);
bool checkNearlyChar(vector <Point> a, vector <Point> b);
double distanceBetweenChars(vector <Point> firstChar, vector <Point> secondChar);
vector <vector <Point> > findGroupsOfChars(vector <vector <Point> > allChars);
string recognizeChars(vector <vector <Point> >& groupOfChars, Mat& imgThresh);
bool checkInvalid(string& plate);
string fixPlateNumber(string& plate);
char convertToNumber(char c);
char convertToLetter(char c);