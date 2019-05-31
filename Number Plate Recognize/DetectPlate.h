#pragma once

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include "preprocess.h";
#include "ExternVariable.h";

const double PLATE_WIDTH_PADDING_FACTOR = 1.3;
const double PLATE_HEIGHT_PADDING_FACTOR = 1.5;

const int MIN_PIXEL_WIDTH = 2;
const int MIN_PIXEL_HEIGHT = 8;

const double MIN_ASPECT_RATIO = 0.25;
const double MAX_ASPECT_RATIO = 1.0;

const int MIN_PIXEL_AREA = 80;

const double MIN_DIAG_SIZE_MULTIPLE_AWAY = 0.3;
const double MAX_DIAG_SIZE_MULTIPLE_AWAY = 3.0;
const double MAX_ANGLE_BETWEEN_CHARS = 12.0;

const double MAX_CHANGE_IN_AREA = 0.5;

const double MAX_CHANGE_IN_WIDTH = 0.8;
const double MAX_CHANGE_IN_HEIGHT = 0.5;

const int MIN_NUMBER_OF_NEARLY_CHARS = 5;

vector<PossiblePlate> detectPlates(Mat& imgOriginalScene);
vector <PossibleChar> findAllChars(Mat& imgThresh);
bool checkPossibleChar(PossibleChar pchar);
vector <vector <PossibleChar> > findGroupsOfChars(vector <PossibleChar> allChars);
bool checkNearlyChar(PossibleChar a, PossibleChar b);
double angleBetweenChars(const PossibleChar& firstChar, const PossibleChar& secondChar);
double distanceBetweenChars(const PossibleChar& firstChar, const PossibleChar& secondChar);
PossiblePlate extractPlate(Mat& imgOriginal, vector<PossibleChar>& vectorOfMatchingChars);
PossiblePlate extractPlate2(Mat& imgOriginal, vector<PossibleChar>& groupOfChars);