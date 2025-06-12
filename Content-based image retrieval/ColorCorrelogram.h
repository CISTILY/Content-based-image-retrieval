#pragma once
#include"Features.h"

class ColorCorrelogram : public Feature {
private:
	vector<Point> neighborPixels;

public:
	ColorCorrelogram() {}
	~ColorCorrelogram() {}

	int colorQuantization(Vec3b);
	bool isNeighbor(int, int, int, int);
	void getNeighborPixels(int, int, int, int, int);
	void createFeature(String, Mat) override;
	void showFeature(String) override;
};