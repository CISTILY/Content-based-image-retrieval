#pragma once
#include "Features.h"
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

class ColorHistogram : public Feature {
private:
	Mat r_hist;
	Mat g_hist;
	Mat b_hist;
public:
	void createFeature(String, Mat) override;
	void showFeature(String) override;
};