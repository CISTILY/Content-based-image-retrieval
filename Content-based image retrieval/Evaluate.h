#pragma once
#include <vector>
#include "Image.h"
#include "ImageDatabase.h"

using namespace std;

class Evaluator {
private:
	vector<double> averagePrecisions;
	double mAP;

public:
	Evaluator() {}
	~Evaluator() {}

	void calculateAveragePrecision(vector<pair<string, float>>, string);
	void calculateMeanAveragePrecision();
	float getMAP();
};