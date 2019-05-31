#include "DetectChars.h"

bool cmp(vector <Point> a, vector <Point> b) {
	Rect boundingA = boundingRect(a);
	Rect boundingB = boundingRect(b);

	int minHeightA = 10000, maxHeightA = 0;
	int minHeightB = 10000, maxHeightB = 0;
	int minWidthA = 10000, minWidthB = 10000;

	for (int i = 0; i < a.size(); i++) {
		minHeightA = min(minHeightA, a[i].y);
		maxHeightA = max(maxHeightA, a[i].y);

		minWidthA = min(minWidthA, a[i].x);
	}

	for (int i = 0; i < b.size(); i++) {
		minHeightB = min(minHeightB, b[i].y);
		maxHeightB = max(maxHeightB, b[i].y);
		minWidthB = min(minWidthB, b[i].x);
	}

	double distanceY = (abs(minHeightA - minHeightB)) * 1.0;
	double sameLineDistance = (abs(maxHeightA - minHeightA)) * SAME_LINE_RATIO;

	//cout << distanceY << ' ' << sameLineDistance << endl;
	if (distanceY > sameLineDistance) return (minHeightA < minHeightB);
	else return (minWidthA < minWidthB);
}

void initKnearest() {
	Mat matClassificationInts; 
	FileStorage fsClassifications("classifications.xml", FileStorage::READ);       
	if (fsClassifications.isOpened() == false) {                                          
		cout << "error, unable to open training classifications file, exiting program\n\n";  
	}
	fsClassifications["classifications"] >> matClassificationInts;     
	fsClassifications.release();                                        

	Mat matTrainingImagesAsFlattenedFloats;     
	FileStorage fsTrainingImages("images.xml", FileStorage::READ);      
	if (fsTrainingImages.isOpened() == false) {                                                
		cout << "error, unable to open training images file, exiting program\n\n";        
	}

	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;         
	fsTrainingImages.release();                                                

	kNearest->train(matTrainingImagesAsFlattenedFloats, ml::ROW_SAMPLE, matClassificationInts);
}

vector <PossibleChar> findCharsInPlate(Mat& imgThresh) {
	vector <PossibleChar> allChars;

	Mat imgThreshClone = imgThresh.clone();

	vector <vector <Point> > contours;
	vector<Vec4i> v4iHierarchy;
	findContours(imgThreshClone, contours, v4iHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//cout << contours.size() << endl;

	for (int i = 0; i < contours.size();i++) {
		PossibleChar pchar(contours[i]);

		//debug
		Mat imgContours = Mat(imgThresh.size(), CV_8UC1, SCALAR_BLACK);
		for (int j = 1; j < contours[i].size(); j++) {
			line(imgContours, contours[i][j], contours[i][j - 1], SCALAR_WHITE);
		}
		line(imgContours, contours[i][0], contours[i][contours[i].size() - 1], SCALAR_WHITE);
		//imshow(to_string(i), imgContours);
		//

		if (checkPossibleChar(pchar) == true) {
			//cout << 1 << endl;
			allChars.push_back(pchar);
		}
	}
	return allChars;
}


vector <vector <Point> > detectChars(Mat &imgPlate) {
	Mat imgGrayscale;
	Mat imgThresh;

	cvtColor(imgPlate, imgGrayscale, CV_RGB2GRAY);
	threshold(imgGrayscale, imgThresh, 127.0, 255.0, THRESH_BINARY_INV);

	imshow("imgthresh", imgThresh);

	vector <PossibleChar> allChars = findCharsInPlate(imgThresh);

	vector <vector <PossibleChar> > groupsOfChars = findGroupsOfChars(allChars);

	//cout << allChars.size() <<endl;

	vector <vector <Point> > charsOfPlate;

	int maxSizeOfGroups = 0;
	for (int i = 1; i < groupsOfChars.size(); i++) {
		if (groupsOfChars[i].size() > groupsOfChars[maxSizeOfGroups].size()) maxSizeOfGroups = i;
	}

	if(groupsOfChars.size() > 0)
	for (int i = 0; i < groupsOfChars[maxSizeOfGroups].size(); i++) {
		charsOfPlate.push_back(groupsOfChars[maxSizeOfGroups][i].contour);
	}
	return charsOfPlate;
}

string recognizeChar(vector <vector <Point> > &charsOfPlate, Mat& imgThresh) {
	string strFinalString = "";

	sort(charsOfPlate.begin(), charsOfPlate.end(), cmp);

	RNG rng;
	int gt = rng.uniform(1, 10);

	//imshow("imgThresh 2", imgThresh);

	Mat imgContours = Mat(imgThresh.size(), CV_8UC1, SCALAR_BLACK);
	fillPoly(imgContours, charsOfPlate, SCALAR_WHITE);

	imshow("imgContours", imgContours);

	//cout << charsOfPlate.size() << endl;

	for (int i = 0; i < charsOfPlate.size(); i++) {

		/*for (int j = 1; j < charsOfPlate[i].size(); j++) {
			line(imgContours, charsOfPlate[i][j], charsOfPlate[i][j - 1], SCALAR_WHITE);
		}
		line(imgContours, charsOfPlate[i][0], charsOfPlate[i][charsOfPlate[i].size() - 1], SCALAR_WHITE);*/

		string name = "imgContour ";
		name += to_string(gt);
		name += ' ';
		name += to_string(i);

		Rect bounding = boundingRect(charsOfPlate[i]);

		//Mat matROI = imgThresh(bounding);
		Mat matROI = imgContours(bounding);

		Mat matROIResized;
		resize(matROI, matROIResized, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));

		//imshow(name, matROI);

		Mat matROIFloat;
		matROIResized.convertTo(matROIFloat, CV_32FC1);
		Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
		Mat matCurrentChar(0, 0, CV_32F);

		kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);

		float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
		strFinalString = strFinalString + char(int(fltCurrentChar));
	}
	//cout << strFinalString << endl;


	return strFinalString;
}

string detectChar(Mat& imgPlate) {
	Mat imgGrayscale;
	Mat imgThresh;

	//cvtColor(imgPlate, imgGrayscale, CV_RGB2GRAY);
	//threshold(imgGrayscale, imgThresh, 127.0, 255.0, THRESH_BINARY);

	preprocess(imgPlate, imgGrayscale, imgThresh);
	//imshow("imgthresh", imgThresh);

	vector <vector <Point> > allChars = detectChars(imgPlate);
	//cout << 1 << endl;
	return recognizeChar(allChars, imgThresh);
}


//string detectChar(Mat imgPlate) {
//	string strFinalString;
//
//	Mat imgGrayscale;
//	Mat imgThresh;
//	preprocess(imgPlate, imgGrayscale, imgThresh);
//	Mat imgThreshClone = imgThresh.clone();
//	
//	vector <vector <Point> > contours;
//	vector<Vec4i> v4iHierarchy;
//	findContours(imgThreshClone, contours, v4iHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//	sort(contours.begin(), contours.end(), cmp);
//
//	//debug
//	Mat imgContours = Mat(imgPlate.size(), CV_8UC3, SCALAR_BLACK);
//	for (int i = 0; i < contours.size(); i++) {
//		for (int j = 1; j < contours[i].size(); j++) {
//			line(imgContours, contours[i][j], contours[i][j - 1], SCALAR_WHITE);
//		}
//	}
//	imshow("imgPlate Contours", imgContours);
//	//
//
//	for (int i = 0; i < contours.size(); i++) {
//		Rect bounding = boundingRect(contours[i]);
//		Mat matROI = imgThresh(bounding);       
//
//		Mat matROIResized;
//		resize(matROI, matROIResized, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));
//
//		Mat matROIFloat;
//		matROIResized.convertTo(matROIFloat, CV_32FC1);
//		Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
//		Mat matCurrentChar(0, 0, CV_32F);
//
//		kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);
//
//		float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
//		strFinalString = strFinalString + char(int(fltCurrentChar));
//	}
//	cout << strFinalString << endl;
//
//	return strFinalString;
//}

bool checkInvalid(string plate) {
	 return true;
}
