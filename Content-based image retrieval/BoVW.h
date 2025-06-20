#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

/**
 * @class BagOfVisualWord
 * @brief A class implementing the Bag of Visual Words (BoVW) model for image representation.
 *
 * This class allows building a visual vocabulary from local image features (e.g., SIFT descriptors),
 * and converting an image's local descriptors into a fixed-length histogram based on the vocabulary.
 */
class BagOfVisualWord {
private:
    Mat vocabulary;       ///< The visual vocabulary (each row is a visual word / cluster center).
    int dictionarySize;   ///< Number of visual words (clusters) in the vocabulary.

public:
    /**
     * @brief Constructor to initialize with a given dictionary size.
     * @param dictSize Number of visual words (default is 100).
     */
    BagOfVisualWord(int dictSize = 100) : dictionarySize(dictSize) {};

    /**
     * @brief Constructor to initialize with a precomputed vocabulary.
     * @param vocabulary A matrix where each row is a visual word.
     */
    BagOfVisualWord(Mat vocabulary) : vocabulary(vocabulary) {};

    /**
     * @brief Destructor.
     */
    ~BagOfVisualWord() {};

    /**
     * @brief Builds the vocabulary using clustering (e.g., k-means) from a list of descriptors.
     * @param descriptors A vector of Mat, where each Mat contains local descriptors from one image.
     *
     * The function merges all descriptors and clusters them into `dictionarySize` visual words.
     */
    void buildVocabulary(vector<Mat>& descriptors);

    /**
     * @brief Computes the BoVW histogram for a given descriptor set using the current vocabulary.
     * @param descriptors A matrix of descriptors extracted from an image (each row is one descriptor).
     * @return A normalized histogram (Mat) representing the frequency of visual words.
     */
    Mat computeHistogram(const Mat& descriptors);

    /**
     * @brief Gets the current visual vocabulary.
     * @return A Mat where each row is a visual word (cluster center).
     */
    Mat getVocabulary() const;

    /**
     * @brief Sets the visual vocabulary manually.
     * @param vocab A matrix where each row represents a visual word.
     */
    void setVocabulary(Mat vocab);
};
