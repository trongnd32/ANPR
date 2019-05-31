#include "DetectPlate.h"

vector<PossiblePlate> detectPlates(Mat& imgOriginal) {
	vector <PossiblePlate> plates;

	Mat imgGrayscale;
	Mat imgThresh;

	preprocess(imgOriginal, imgGrayscale, imgThresh);

	//imshow("imgOThresh",imgThresh);

	vector <PossibleChar> allChars = findAllChars(imgThresh);

	vector <vector <PossibleChar> > groupsOfChars = findGroupsOfChars(allChars);

	// debug
	for (int i = 0; i < groupsOfChars.size(); i++) {
		string name = "group of char ";
		name += to_string(i);
		Mat imgContours = Mat(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);
		for (int k = 0; k < groupsOfChars[i].size(); k++) {
			for (int j = 1; j < groupsOfChars[i][k].contour.size(); j++) {
				line(imgContours, groupsOfChars[i][k].contour[j], groupsOfChars[i][k].contour[j - 1], SCALAR_WHITE, 2);
			}
			line(imgContours, groupsOfChars[i][k].contour[0], groupsOfChars[i][k].contour[groupsOfChars[i][k].contour.size() - 1], SCALAR_WHITE, 2);
		}
		PossiblePlate plate = extractPlate2(imgContours, groupsOfChars[i]);

		if (plate.imgPlate.empty() == false) {
			plates.push_back(plate);
		}

		//imshow(name, imgContours);
	}
	//

	/*for (auto& group : groupsOfChars) {
		PossiblePlate plate = extractPlate2(imgOriginal, group);

		if (plate.imgPlate.empty() == false) {
			plates.push_back(plate);
		}
	}*/

	// debug
	for (int i = 0; i < allChars.size(); i++) {
		Mat imgContours = Mat(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);
		string name = "char ";
		name += to_string(i);
		for (int j = 1; j < allChars[i].contour.size(); j++) {
			line(imgContours, allChars[i].contour[j], allChars[i].contour[j - 1], SCALAR_WHITE);
		}
		//imshow(name, imgContours);
	}
	//
	return plates;
}

vector <PossibleChar> findAllChars(Mat& imgThresh) {
	vector <PossibleChar> allChars;

	Mat imgThreshClone = imgThresh.clone();

	vector <vector <Point> > contours;
	findContours(imgThreshClone, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size();i++) {
		PossibleChar pchar(contours[i]);

		if (checkPossibleChar(pchar) == true) {
			allChars.push_back(pchar);
		}
	}
	waitKey(0);
	return allChars;
}

bool checkPossibleChar(PossibleChar pchar) {
	double whRatio = ((double)(pchar.boundingRect.width) / pchar.boundingRect.height);

	//cout << pchar.boundingRect.width << ' ' << pchar.boundingRect.height << ' ' << whRatio << endl;

	if (pchar.boundingRect.area() > MIN_PIXEL_AREA && pchar.boundingRect.width > MIN_PIXEL_WIDTH 
		&& pchar.boundingRect.height > MIN_PIXEL_HEIGHT && whRatio > MIN_ASPECT_RATIO 
		&& whRatio < MAX_ASPECT_RATIO) {
		return true;
	}
	return false;
}

vector <vector <PossibleChar> > findGroupsOfChars(vector <PossibleChar> allChars) {
	vector <vector <PossibleChar> > groupsOfChars;

	vector <int> mark;
	int n = allChars.size();
	for (int i = 1; i <= n; i++) mark.push_back(0);
	vector <int> q;
	vector <PossibleChar> aGroup;

	for (int i = 0; i < n; i++) if (mark[i] == 0) {
		q.clear();
		q.push_back(i);
		aGroup.clear();
		for (int j = 0; j < n; j++) if (mark[j] == 0 && i!=j) {
			if (checkNearlyChar(allChars[i], allChars[j]) == true) q.push_back(j);
		}
		if (q.size() >= MIN_NUMBER_OF_NEARLY_CHARS) {
			for (int j = 0; j < q.size(); j++) {
				mark[q[j]] = 1;
				aGroup.push_back(allChars[q[j]]);
			}
			groupsOfChars.push_back(aGroup);
		}
	}
	return groupsOfChars;
}

bool checkNearlyChar(PossibleChar a, PossibleChar b) {
	double dblDistanceBetweenChars = distanceBetweenChars(a, b);
	double dblAngleBetweenChars = angleBetweenChars(a, b);
	double dblChangeInArea = (double)abs(a.boundingRect.area() - b.boundingRect.area()) / (double)b.boundingRect.area();
	double dblChangeInWidth = (double)abs(a.boundingRect.width - b.boundingRect.width) / (double)b.boundingRect.width;
	double dblChangeInHeight = (double)abs(a.boundingRect.height - b.boundingRect.height) / (double)b.boundingRect.height;

	// check
	if (dblDistanceBetweenChars < (a.dblDiagonalSize * MAX_DIAG_SIZE_MULTIPLE_AWAY) &&
		dblChangeInArea < MAX_CHANGE_IN_AREA &&
		dblChangeInWidth < MAX_CHANGE_IN_WIDTH &&
		dblChangeInHeight < MAX_CHANGE_IN_HEIGHT) {
		return true;
	}
	return false;
}

double angleBetweenChars(const PossibleChar& firstChar, const PossibleChar& secondChar) {
	double dblAdj = abs(firstChar.intCenterX - secondChar.intCenterX);
	double dblOpp = abs(firstChar.intCenterY - secondChar.intCenterY);

	double dblAngleInRad = atan(dblOpp / dblAdj);
	double dblAngleInDeg = dblAngleInRad * (180.0 / CV_PI);

	return(dblAngleInDeg);
}

double distanceBetweenChars(const PossibleChar& firstChar, const PossibleChar& secondChar) {
	int intX = abs(firstChar.intCenterX - secondChar.intCenterX);
	int intY = abs(firstChar.intCenterY - secondChar.intCenterY);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PossiblePlate extractPlate2(Mat& imgOriginal, vector<PossibleChar>& groupOfChars) {
	PossiblePlate possiblePlate;
	int maxHeight = 0, minHeight = 100000, maxWidth = 0, minWidth = 100000;

	double dblPlateCenterX = 0, dblPlateCenterY = 0;
	int plateWidth, plateHeight;

	for (int i = 0; i < groupOfChars.size(); i++) {
		minWidth = min(groupOfChars[i].boundingRect.x, minWidth);
		maxWidth = max(groupOfChars[i].boundingRect.x + groupOfChars[i].boundingRect.width, maxWidth);

		minHeight = min(groupOfChars[i].boundingRect.y, minHeight);
		maxHeight = max(groupOfChars[i].boundingRect.y + groupOfChars[i].boundingRect.height, maxHeight);
	}

	minWidth -= 10;
	maxWidth += 10;
	minHeight -= 10;
	maxHeight += 10;

	dblPlateCenterX = (minWidth + maxWidth) / 2.0;
	dblPlateCenterY = (minHeight + maxHeight) / 2.0;
	plateWidth = maxWidth - minWidth;
	plateHeight = maxHeight - minHeight;

	Point2d p2dPlateCenter(dblPlateCenterX, dblPlateCenterY);

	vector <int> intAngles;

	for (int i = 0; i < groupOfChars.size(); i++) {
		for (int j = i + 1; j < groupOfChars.size(); j++) {
			double dblOpposite = abs(groupOfChars[i].intCenterY - groupOfChars[j].intCenterY);
			double dblHypotenuse = distanceBetweenChars(groupOfChars[i], groupOfChars[j]);
			double dblCorrectionAngleInRad = asin(dblOpposite / dblHypotenuse);
			double dblCorrectionAngleInDeg = abs(dblCorrectionAngleInRad * (180.0 / CV_PI));
			//if (groupOfChars[i].intCenterY > groupOfChars[j].intCenterY) dblCorrectionAngleInDeg = dblCorrectionAngleInDeg * (-1.0);
			int intCorrectionAngleInDeg = int(dblCorrectionAngleInDeg + 0.5);
			intAngles.push_back(intCorrectionAngleInDeg);
		}
	}
	sort(intAngles.begin(), intAngles.end());

	int dem = 1, slMax = 0, gtMax = 0;
	for (int i = 1; i < intAngles.size(); i++) {
		if (intAngles[i] != intAngles[i - 1]) {
			if (dem > slMax) {
				slMax = dem;
				gtMax = intAngles[i - 1];
			}
			dem = 1;
		}
	}
	if (dem > slMax) {
		slMax = dem;
		gtMax = intAngles[intAngles.size()];
	}

	double dblCorrectionAngleInDeg = 0.0;
	int tt = 0;

	for (int i = 0; i < groupOfChars.size(); i++) {
		if (tt == 1) break;
		for (int j = i + 1; j < groupOfChars.size(); j++) {
			double dblOpposite = abs(groupOfChars[i].intCenterY - groupOfChars[j].intCenterY);
			double dblHypotenuse = distanceBetweenChars(groupOfChars[i], groupOfChars[j]);
			double dblCorrectionAngleInRad = asin(dblOpposite / dblHypotenuse);
			dblCorrectionAngleInDeg = abs(dblCorrectionAngleInRad * (180.0 / CV_PI));
			//if (groupOfChars[i].intCenterY > groupOfChars[j].intCenterY) dblCorrectionAngleInDeg = dblCorrectionAngleInDeg * (-1.0);
			int intCorrectionAngleInDeg = int(dblCorrectionAngleInDeg + 0.5);
			if (gtMax == intCorrectionAngleInDeg) {
				double diffY = (groupOfChars[i].intCenterY - groupOfChars[j].intCenterY);
				double diffX = (groupOfChars[i].intCenterX - groupOfChars[j].intCenterX);
				if (diffY * diffX < 0) dblCorrectionAngleInDeg = dblCorrectionAngleInDeg*(-1.0);
				tt = 1;
				break;
			}
		}
	}
	
	//cout << dblCorrectionAngleInDeg << endl;

	possiblePlate.rrLocationOfPlateInScene = RotatedRect(p2dPlateCenter, Size2f((float)plateWidth, (float)plateHeight), (float)dblCorrectionAngleInDeg);

	Mat rotationMatrix;         
	Mat imgRotated;
	Mat imgCropped;

	rotationMatrix = getRotationMatrix2D(p2dPlateCenter, dblCorrectionAngleInDeg, 1.0);
	warpAffine(imgOriginal, imgRotated, rotationMatrix, imgOriginal.size());      
	//imshow("imgRotated", imgRotated);
	getRectSubPix(imgRotated, possiblePlate.rrLocationOfPlateInScene.size, possiblePlate.rrLocationOfPlateInScene.center, imgCropped);

	possiblePlate.imgPlate = imgCropped; 

	resize(possiblePlate.imgPlate, possiblePlate.imgPlate, Size(), 2.0, 2.0);

	return(possiblePlate);
}