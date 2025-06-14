#pragma once
#include <opencv2/opencv.hpp>
#include <unordered_set>
#include "Image.h"
#include "ColorHistogram.h"
#include "Distances.h"
#include "FeatureDatabase.h"
#include "HOG.h"
#include "ORB.h"
#include "SIFT.h"
#include <string>
#include "ColorCorrelogram.h"
#include "BoVW.h"

using namespace std;
using namespace cv;

class Query {
private:
	Image QueryImage;
	vector<pair<string, float>> results;
	bool useSimilarity;
public:
	void Search(string, Mat, map<int, vector<string>>& index, map<int, vector<float>>& centroids, map<int, vector<Feature*>>&, Mat&, int, string);
	vector<pair<string, float>> getResult();
};