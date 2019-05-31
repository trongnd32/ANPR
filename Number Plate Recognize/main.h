#pragma once

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include <cstdio>
#include<string>
#include<conio.h> 
#include <ctime>

#include "preprocess.h";
#include "DetectPlate2.h";
#include "DetectChar2.h";
#include "ExternVariable.h";

using namespace std;
using namespace cv;

Ptr<ml::KNearest>  kNearest(ml::KNearest::create());
