#pragma once
#include <math.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Distance {
public:
	float calculateCosineDistance(vector<float>, vector<float>);
	float calculateL1Distance(vector<float>, vector<float>);
	float calculateL2Distance(Mat, Mat);
	float averageMinL2Distance(Mat, Mat);
};
