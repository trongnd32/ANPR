#include "main.h"

int main() {

	FILE* stream;
	//freopen_s(&stream, "output2.txt", "w", stdout);
	initKnearest();

	vector <string> setOfNumber;

	//for (int o = 1; o <= 244; o++) {
	//	clock_t start = clock();

	//	Mat imgOriginal;
	//	string filePath = "Bike_back/";
	//	filePath += to_string(o);
	//	filePath += ".jpg";
	//	//imgOriginal = imread("invalid image/plate_4.jpg");
	//	imgOriginal = imread(filePath);

	//	if (imgOriginal.empty()) {
	//		cout << "error: image not read from file\n\n";
	//		_getch();
	//		return(0);
	//	}

	//	vector <Mat> plates;

	//	plates = PlateFinder(imgOriginal);

	//	for (int i = 0; i < plates.size(); i++) {
	//		//imshow(to_string(i), plates[i]);
	//	}

	//	setOfNumber.clear();

	//	for (int i = 0; i < plates.size(); i++) {
	//		string plateNumber = detectChars(plates[i]);
	//		//imshow(to_string(i),plates[i]);
	//		if (checkInvalid(plateNumber) == true) {
	//			setOfNumber.push_back(plateNumber);
	//			//cout << plateNumber << " ";
	//		}
	//	}

	//	int maxLen = 0;
	//	if (setOfNumber.empty() == false){
	//		for (int i = 1; i < setOfNumber.size(); i++) if (setOfNumber[i].length() > setOfNumber[maxLen].length()) maxLen = i;
	//		cout << setOfNumber[maxLen] << ' ';
	//	}
	//	cout << (double)(clock() - start) / CLOCKS_PER_SEC;
	//	cout << endl;
	//	waitKey(0);
	//
	//}


	clock_t start = clock();

	Mat imgOriginal;
	string filePath = "";
	//cin >> filePath;

	filePath = "Bike_back/2.jpg";
	
	imgOriginal = imread(filePath);

	if (imgOriginal.empty()) {
		cout << "error: image not read from file\n\n";
		_getch();
		return(0);
	}

	vector <Mat> plates;

	plates = PlateFinder(imgOriginal);

	for (int i = 0; i < plates.size(); i++) {
		imshow(to_string(i), plates[i]);
	}

	setOfNumber.clear();

	for (int i = 0; i < plates.size(); i++) {
		string plateNumber = detectChars(plates[i]);
		//imshow(to_string(i),plates[i]);
		if (checkInvalid(plateNumber) == true) {
			setOfNumber.push_back(plateNumber);
			//cout << plateNumber << " ";
		}
	}

	int maxLen = 0;
	if (setOfNumber.empty() == false) {
		for (int i = 1; i < setOfNumber.size(); i++) if (setOfNumber[i].length() > setOfNumber[maxLen].length()) maxLen = i;
		cout << setOfNumber[maxLen] << ' ';
	}
	//cout << (double)(clock() - start) / CLOCKS_PER_SEC;
	cout << endl;
	waitKey(0);

	return 0;
}