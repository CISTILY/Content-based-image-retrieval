#pragma once
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

class Feature {
protected:
	String id;
	Mat imageDescriptors;
public:
	// Constructor
	Feature() {}

	// Destructor
	virtual ~Feature() {}

	// Getters
	String getId();
	virtual Mat getDescriptor() const { return imageDescriptors; }

	// Setters
	void setId(const string& newId) { id = newId; }
	void setDescriptor(const Mat& newContent) { imageDescriptors = newContent; }

	// Virtual functions
	void virtual createFeature(String, Mat) = 0;
	void virtual showFeature(String) = 0;
};