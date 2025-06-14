#pragma once
#include <math.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Distance {
public:
	float calculateSimilarity(Mat, Mat, string);
	float calculateDistance(Mat, Mat, string);
};
