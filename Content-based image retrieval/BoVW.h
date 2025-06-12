#pragma once

#include<opencv2/opencv.hpp>
#include<vector>

using namespace std;
using namespace cv;

class BagOfVisualWord {
private:
	Mat vocabulary;
	int dictionarySize;

public:
	BagOfVisualWord(int dictSize = 100) : dictionarySize(dictSize) {}
	BagOfVisualWord(Mat vocabulary) : vocabulary(vocabulary) {}
	~BagOfVisualWord() {}

	void buildVocabulary(vector<Mat>&);
	Mat computeHistogram(const Mat&);

	Mat getVocabulary() const;
	void setVocabulary(Mat);
};