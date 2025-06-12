#pragma once
#include "Features.h"
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

class HOG : public Feature {
public:
	void createFeature(String, Mat) override;
	void showFeature(String) override;
	Mat computeHOG(InputArray, InputArray, bool);
};