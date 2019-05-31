#include "DetectChar2.h";

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
	double sameLineDistance = (max(abs(maxHeightA - minHeightA), abs(maxHeightB - minHeightB))) * SAME_LINE_RATIO;

	//cout << distanceY << ' ' << sameLineDistance << endl;
	//cout << minHeightA << ' ' << minWidthA << ' ' << maxHeightA << ' ' << minHeightB << ' ' << minWidthB << ' ' << maxHeightB << endl;
	if (distanceY >= sameLineDistance) return (minHeightA < minHeightB);
	return (minWidthA < minWidthB);
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

string detectChars(Mat& imgPlate) {
	string strPlate = "";

	Mat imgGrayscale;
	Mat imgThresh;

	preprocess(imgPlate, imgGrayscale, imgThresh);
	Mat imgThreshClone = imgThresh.clone();
	Mat imgThreshFinal = imgThresh.clone();

	//resize plate
	double fx = PLATE_WIDTH_SIZE * 1.0 / imgThresh.cols;
	if(fx > 1.4) resize(imgThreshClone, imgThresh, Size(), fx, fx);

	// Phep gian, co
	//imshow("imgThresh", imgThresh);
	imgThreshClone = imgThresh.clone();
	Mat M = Mat::ones(5, 5, CV_8U);
	//morphologyEx(imgThreshClone, imgThreshFinal, MORPH_OPEN, M);
	//morphologyEx(imgThreshClone, imgThreshFinal, MORPH_CLOSE, M);
	//imshow("imgThresh 2", imgThreshFinal);
	//

	vector <vector <Point> > contours;
	vector<Vec4i> v4iHierarchy;

	findContours(imgThreshFinal, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

	vector <vector <Point> > possibleChars = removeImpossibleChars(contours);
	//contours.clear();

	//cout << possibleChars.size() << endl;

	//imshow("contours", imgContours);

	vector <vector <Point> > groupOfChars = findGroupsOfChars(possibleChars);
	//possibleChars.clear();
	
	Mat imgContours = Mat(imgThresh.size(), CV_8UC1, SCALAR_BLACK);
	for (int i = 0; i < possibleChars.size(); i++) {
		//for (int j = 1; j < possibleChars[i].size(); j++) line(imgContours, possibleChars[i][j], possibleChars[i][j - 1], SCALAR_WHITE);
		Rect bounding = boundingRect(possibleChars[i]);

		Mat matROI = imgThresh(bounding);
		//Mat matROI = imgContours(bounding);

		Mat matROIResized;
		resize(matROI, matROIResized, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));
		//imshow(to_string(i) + "a", matROIResized);
		//cout << i << ' ' << bounding.area() << ' ' << bounding.height << ' ' << bounding.width << endl;
	}

	//cout << groupOfChars.size() << endl;

	if (groupOfChars.size() < MIN_NUMBER_OF_NEARLY_CHARS) return strPlate;

	//cout << groupOfChars.size() << endl;
	//cout << 1 << endl;
	//Mat imgContours = Mat(imgThresh.size(), CV_8UC1, SCALAR_BLACK);
	//imshow("contours", imgContours);

	sort(groupOfChars.begin(), groupOfChars.end(), cmp);

	return recognizeChars(groupOfChars, imgThreshFinal);
}

vector <vector <Point> > removeImpossibleChars(vector <vector <Point> > contours) {
	vector <vector <Point> > possibleChars;

	for (int i = 0; i < contours.size(); i++) {
		if (checkPossibleChar(contours[i]) == true) possibleChars.push_back(contours[i]);
	}
	
	return possibleChars;
}

bool checkPossibleChar(vector <Point> pchar) {
	Rect bounding = boundingRect(pchar);
	double whRatio = ((double)(bounding.width) / bounding.height);

	if (bounding.area() > MIN_PIXEL_AREA && bounding.width > MIN_PIXEL_WIDTH
		&& bounding.height > MIN_PIXEL_HEIGHT && whRatio > MIN_ASPECT_RATIO
		&& whRatio < MAX_ASPECT_RATIO) {
		return true;
	}
	return false;
}

vector <vector <Point> > findGroupsOfChars(vector <vector <Point> > allChars) {
	vector <vector <vector <Point> > > groupsOfChars;

	vector <int> mark;
	int n = allChars.size();
	for (int i = 1; i <= n+5; i++) mark.push_back(0);
	vector <int> q;
	vector < vector <Point> > aGroup;

	for (int i = 0; i < n; i++) if (mark[i] == 0) {
		q.clear();
		q.push_back(i);
		aGroup.clear();
		for (int j = 0; j < n; j++) if (mark[j] == 0 && i != j) {
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

	int maxSizeOfGroups = 0;
	for (int i = 1; i < groupsOfChars.size(); i++) {
		if (groupsOfChars[i].size() > groupsOfChars[maxSizeOfGroups].size()) maxSizeOfGroups = i;
	}

	if (groupsOfChars.size() <= maxSizeOfGroups) return {};

	return groupsOfChars[maxSizeOfGroups];
}

bool checkNearlyChar(vector <Point> a, vector <Point> b) {
	Rect boundingA = boundingRect(a);
	Rect boundingB = boundingRect(b);

	double dblDistanceBetweenChars = distanceBetweenChars(a, b);
	double dblChangeInArea = (double)abs(boundingA.area() - boundingB.area()) / (double)boundingB.area();
	double dblChangeInWidth = (double)abs(boundingA.width - boundingB.width) / (double)boundingB.width;
	double dblChangeInHeight = (double)abs(boundingA.height - boundingB.height) / (double)boundingB.height;
	double dblDiagonalSize = sqrt(pow(boundingA.width, 2) + pow(boundingA.height, 2));

	// check
	if (dblDistanceBetweenChars < (dblDiagonalSize * MAX_DIAG_SIZE_MULTIPLE_AWAY) &&
		dblChangeInArea < MAX_CHANGE_IN_AREA &&
		dblChangeInWidth < MAX_CHANGE_IN_WIDTH &&
		dblChangeInHeight < MAX_CHANGE_IN_HEIGHT) {
		return true;
	}
	return false;
}

double distanceBetweenChars(vector <Point> firstChar, vector <Point> secondChar) {
	Rect boundingA = boundingRect(firstChar);
	Rect boundingB = boundingRect(secondChar);

	int firstX = (boundingA.x + boundingA.x + boundingA.width) / 2;
	int firstY = (boundingA.y + boundingA.y + boundingA.height) / 2;

	int secondX = (boundingB.x + boundingB.x + boundingB.width) / 2;
	int secondY = (boundingB.y + boundingB.y + boundingB.height) / 2;

	int intX = abs(firstX - secondX);
	int intY = abs(firstY - secondY);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

string recognizeChars(vector <vector <Point> >& groupOfChars, Mat& imgThresh) {
	string strFinalString = "";

	int preCenterX = -1000;
	int preCenterY = -1000;

	for (int i = 0; i < groupOfChars.size(); i++) {
		Rect bounding = boundingRect(groupOfChars[i]);

		Mat matROI = imgThresh(bounding);
		//Mat matROI = imgContours(bounding);

		Mat M = Mat::ones(5, 5, CV_8U);
		//morphologyEx(matROI, matROI, MORPH_CLOSE, M);

		Mat matROIResized;
		resize(matROI, matROIResized, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));

		string name = "char ";
		name += to_string(i);

		//imshow(name, matROI);

		Mat matROIFloat;
		matROIResized.convertTo(matROIFloat, CV_32FC1);
		Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
		Mat matCurrentChar(0, 0, CV_32F);

		int centerX = (bounding.x + bounding.x + bounding.width) / 2;
		int centerY = (bounding.y + bounding.y + bounding.height) / 2;
		//cout << centerX << ' ' << centerY << endl;

		if ((abs(preCenterX - centerX) > CHAR_DISTANCE_X || abs(preCenterY - centerY) > CHAR_DISTANCE_Y)) {
			kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);

			float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
			strFinalString = strFinalString + char(int(fltCurrentChar));
			imshow(name, matROI);
		}

		preCenterX = centerX;
		preCenterY = centerY;
	}

	return strFinalString;
}

bool checkInvalid(string &plate) {
	int n = plate.length();
	if (n >= 6 && n <= 10) {
		plate = fixPlateNumber(plate);
		return true;
	}
	return false;
}

string fixPlateNumber(string &plate) {
	int n = plate.length();
	for (int i = 0; i <= 1; i++) {
		if (!(plate[i] <= '9' && plate[i] >= '0')) plate[i] = convertToNumber(plate[i]);
	}

	for (int i = n - 1; i >= n - 5; i--) {
		if (!(plate[i] <= '9' && plate[i] >= '0')) plate[i] = convertToNumber(plate[i]);
	}
	if (plate[2] <= '9' && plate[2] >= '0') plate[2] = convertToLetter(plate[2]);
	if (n == 10) {
		if (plate[3] <= '9' && plate[3] >= '0') plate[3] = convertToLetter(plate[3]);
	}
	if (n == 9) if (!(plate[n - 6] <= '9' && plate[n - 6] >= '0')) plate[n - 6] = convertToNumber(plate[n - 6]);
	return plate;
}

char convertLetterToNumber[] = { '4','8','6','0','8','7','6','4','1','9','3','2','0','1','0','2','0','3','5','1','0','7','W','1','7','2' };
char convertNumberToLetter[] = { 'U','I','D','B','A','S','G','I','B','S' };

char convertToNumber(char c) {
	return convertLetterToNumber[c - 'A'];
}

char convertToLetter(char c) {
	return convertNumberToLetter[c - '0'];
}