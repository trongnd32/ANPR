#include "DetectPlate2.h";

vector <Mat> PlateFinder(Mat &imgOriginal) {
	vector <Mat> foundPlate;

	Mat imgGrayscale, imgThresh;

	preprocess(imgOriginal, imgGrayscale, imgThresh);

	Mat imgThreshClone = imgThresh.clone();
	Mat M = Mat::ones(5, 5, CV_8U);
	morphologyEx(imgThreshClone, imgThresh, MORPH_CLOSE, M);


	imshow("imgThresh", imgThresh);

	vector <vector <Point> > contours;
	findContours(imgThresh, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
		RotatedRect rotatedRect = minAreaRect(contours[i]);

		//cout << rotatedRect.size.width << ' ' << rotatedRect.size.height << "\n";

		if (checkInvalidPlate(rotatedRect) == true) {
			Mat rotationMatrix;
			Mat imgRotated;
			Mat imgCropped;

			rotationMatrix = getRotationMatrix2D(rotatedRect.center, rotatedRect.angle, 1.0);
			//imshow("imgOrigin", imgOriginal);
			warpAffine(imgOriginal, imgRotated, rotationMatrix, imgOriginal.size());
			//imshow("imgRotated", imgRotated);
			//Size size = Size(rotatedRect.size.height,rotatedRect.size.width);
			getRectSubPix(imgRotated, rotatedRect.size, rotatedRect.center, imgCropped);
			foundPlate.push_back(imgCropped);
		}
	}

	return foundPlate;
}

bool checkInvalidPlate(RotatedRect &rotatedRect) {
	float rectWidth = rotatedRect.size.width;
	float rectHeight = rotatedRect.size.height;

	if (rectWidth < rectHeight) {
		//heightWidthRatio = 1.0 / heightWidthRatio;
		swap(rectWidth, rectHeight);
		swap(rotatedRect.size.width, rotatedRect.size.height);
		rotatedRect.angle += 90;
	}

	float heightWidthRatio = rectHeight * 1.0 / rectWidth;

	//if (rectWidth >= 200) cout << heightWidthRatio << ' ' << rectWidth << ' ' << rectHeight << ' '<< rotatedRect.angle<<endl;

	if(rectWidth > MIN_PLATE_WIDTH && rectWidth < MAX_PLATE_WIDTH && 
		rectHeight > MIN_PLATE_HEIGHT && rectHeight < MAX_PLATE_HEIGHT
		&& heightWidthRatio > MIN_PLATE_RATIO && heightWidthRatio < MAX_PLATE_RATIO) return true;
	return false;
}