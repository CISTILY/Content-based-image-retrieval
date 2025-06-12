#pragma once
#include "Features.h"
#include "ImageDatabase.h"
#include <fstream>

using namespace std;

class FeatureDatabase {
private:
	vector<Feature*> features;
public:
	FeatureDatabase() {}
	~FeatureDatabase();

	void extractColorHistogram(ImageDatabase);
	void extractHOG(ImageDatabase);
	void extractColorCorrelogram(ImageDatabase);
	void extractSIFT(ImageDatabase);
	void extractORB(ImageDatabase);
	void showAllFeatures();
	void createFeatureDatabase(String);
	vector<Feature*> getFeatures();
};