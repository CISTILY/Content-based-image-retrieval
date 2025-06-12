#pragma once
#include "Features.h"

class ORBFeature : public Feature {
public:
	ORBFeature() {}
	~ORBFeature() {}

	void createFeature(String, Mat) override;
	void showFeature(String) override;
};