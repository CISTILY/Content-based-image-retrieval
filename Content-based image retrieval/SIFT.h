#pragma once
#include "Features.h"
#include <opencv2/features2d.hpp>

class SIFTFeature : public Feature {
private:

public:
	SIFTFeature() {}
	~SIFTFeature() {}

	void createFeature(String, Mat) override;
	void showFeature(String) override;
};