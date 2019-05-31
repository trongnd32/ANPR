// GenData.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<vector>

using namespace std;
using namespace cv;

const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;


int convertToAscii(int x) {
	int rs = 0;
	if (x <= 9 && x >= 0) rs = x + 48;
	else rs = 65 + x - 10;
	return rs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

    Mat imgTraining;         // input image
    Mat imgGrayscale;               //
    Mat imgBlurred;                 // declare various images
    Mat imgThresh;                  //
    Mat imgThreshCopy;              //

    vector<vector<Point> > ptContours;        // declare contours vector
    vector<Vec4i> v4iHierarchy;                    // declare contours hierarchy

    Mat matClassificationInts;      // these are our training classifications, note we will have to perform some conversions before writing to file later

                                            // these are our training images, due to the data types that the KNN object KNearest requires, we have to declare a single Mat,
                                            // then append to it as though it's a vector, also we will have to perform some conversions before writing to file later
    Mat matTrainingImagesAsFlattenedFloats;

    // possible chars we are interested in are digits 0 through 9 and capital letters A through Z, put these in vector intValidChars            

	for (int o = 0; o <= 35; o++) {

		string imgTrain = "train data 3/";
		imgTrain += to_string(o);
		imgTrain += ".png";

		imgTraining = imread(imgTrain);

		if (imgTraining.empty()) {
			continue;
		}

		cvtColor(imgTraining, imgGrayscale, CV_BGR2GRAY);
		threshold(imgGrayscale, imgThresh, 127.0, 255.0, THRESH_BINARY_INV);
		//GaussianBlur(imgGrayscale, imgBlurred, Size(5, 5), 0);
		//adaptiveThreshold(imgBlurred, imgThresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, 2);
		imgThreshCopy = imgThresh.clone();
		findContours(imgThreshCopy, ptContours, v4iHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		int intChar = convertToAscii(o);

		imshow("img", imgThresh);

		//cout << ptContours.size()<<' ';

		for (int i = 0; i < ptContours.size(); i++) {                           // for each contour
			if (contourArea(ptContours[i]) > MIN_CONTOUR_AREA) {                // if contour is big enough to consider
				Rect boundingRect = cv::boundingRect(ptContours[i]);                // get the bounding rect

				rectangle(imgTraining, boundingRect, Scalar(0, 0, 255), 2);      // draw red rectangle around each contour as we ask user for input

				Mat matROI = imgThresh(boundingRect);           // get ROI image of bounding rect

				Mat matROIResized;
				resize(matROI, matROIResized, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage

				//imshow("matROI", matROI);                               // show ROI image for reference
				//imshow("matROIResized", matROIResized);                 // show resized ROI image for reference
				//imshow("imgTrainingNumbers", imgTrainingNumbers);       // show training numbers image, this will now have red rectangles drawn on it
				matClassificationInts.push_back(intChar);
				Mat matImageFloat;                        
				matROIResized.convertTo(matImageFloat, CV_32FC1);   
				Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1); 
				matTrainingImagesAsFlattenedFloats.push_back(matImageFlattenedFloat);   
			} 
		}
		cout << "training " << (char)intChar << " completed\n";
	}
    cout << "training complete\n\n";
    FileStorage fsClassifications("classifications.xml", FileStorage::WRITE);           // open the classifications file

    if (fsClassifications.isOpened() == false) {                                                        // if the file was not opened successfully
        cout << "error, unable to open training classifications file, exiting program\n\n";        // show error message
        return(0);                                                                                      // and exit program
    }

    fsClassifications << "classifications" << matClassificationInts;        // write classifications into classifications section of classifications file
    fsClassifications.release();                                            // close the classifications file

                                                                            // save training images to file ///////////////////////////////////////////////////////

    FileStorage fsTrainingImages("images.xml", FileStorage::WRITE);         // open the training images file

    if (fsTrainingImages.isOpened() == false) {                                                 // if the file was not opened successfully
        cout << "error, unable to open training images file, exiting program\n\n";         // show error message
        return(0);                                                                              // and exit program
    }

    fsTrainingImages << "images" << matTrainingImagesAsFlattenedFloats;         // write training images into images section of images file
    fsTrainingImages.release();                                                 // close the training images file

	waitKey(0);

    return(0);
}




