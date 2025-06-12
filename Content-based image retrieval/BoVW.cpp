#include "BoVW.h"
void BagOfVisualWord::buildVocabulary(vector<Mat>& allDescriptors) {
    Mat descriptorsStacked;

    for (const Mat& desc : allDescriptors) {
        if (!desc.empty()) {
            Mat floatDesc;
            if (desc.type() != CV_32F)
                desc.convertTo(floatDesc, CV_32F);
            else
                floatDesc = desc;

            if (floatDesc.dims == 2)
                descriptorsStacked.push_back(floatDesc);
            else
                cerr << "Descriptor with unexpected dimensions, skipping.\n";
        }
    }

    // Validate conditions for kmeans
    if (descriptorsStacked.empty()) {
        cerr << "Error: No descriptors to build vocabulary.\n";
        return;
    }

    if (descriptorsStacked.type() != CV_32F) {
        cerr << "Error: descriptorsStacked is not of type CV_32F.\n";
        return;
    }

    if (dictionarySize <= 0) {
        cerr << "Error: dictionarySize must be greater than 0.\n";
        return;
    }

    if (descriptorsStacked.rows < dictionarySize) {
        cerr << "Error: Not enough descriptors (" << descriptorsStacked.rows
            << ") for dictionary size (" << dictionarySize << ").\n";
        return;
    }

    // kmeans clustering
    Mat temp;
    kmeans(descriptorsStacked, dictionarySize, temp,
        TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 100, 0.01),
        3, KMEANS_PP_CENTERS, vocabulary);
}

Mat BagOfVisualWord::computeHistogram(const Mat& descriptors) {
    Mat hist = Mat::zeros(1, vocabulary.rows, CV_32F);

    for (int i = 0; i < descriptors.rows; ++i) {
        float minDist = FLT_MAX;
        int bestIdx = 0;
        for (int j = 0; j < vocabulary.rows; ++j) {
            float dist = norm(descriptors.row(i), vocabulary.row(j), NORM_L2);
            if (dist < minDist) {
                minDist = dist;
                bestIdx = j;
            }
        }
        hist.at<float>(0, bestIdx) += 1.0f;
    }

    normalize(hist, hist, 1, 0, NORM_L2);
    return hist;
}

Mat BagOfVisualWord::getVocabulary() const { return vocabulary; }

void BagOfVisualWord::setVocabulary(Mat vocab) {
    vocabulary = vocab.clone();
}