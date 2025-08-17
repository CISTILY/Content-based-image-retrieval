#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

/**
 * @class BagOfVisualWord
 * @brief Implements the Bag of Visual Words (BoVW) model for image representation.
 *
 * The BagOfVisualWord class enables building a visual vocabulary from local image descriptors
 * (e.g., SIFT, ORB) and converting an image into a fixed-length histogram representation.
 * It is commonly used in image classification and retrieval systems.
 */
class BagOfVisualWord {
private:
    Mat vocabulary;         ///< Matrix representing the visual vocabulary (each row is a cluster center).
    int dictionarySize;     ///< Number of clusters (visual words) in the vocabulary.

public:
    /**
     * @brief Constructor with user-defined dictionary size.
     *
     * @param[in] dictSize   The number of visual words to use in the vocabulary (default is 100).
     */
    BagOfVisualWord(int dictSize) : dictionarySize(dictSize) {}

    /**
     * @brief Constructor with a precomputed vocabulary.
     *
     * @param[in] vocabulary   Matrix where each row is a visual word (cluster center).
     */
    BagOfVisualWord(Mat vocabulary) : vocabulary(vocabulary) {}

    /**
     * @brief Destructor.
     */
    ~BagOfVisualWord() {}

    /**
     * @brief Builds the visual vocabulary by clustering descriptors using k-means.
     *
     * This method aggregates all descriptors across images, then clusters them into
     * `dictionarySize` visual words. The vocabulary is stored internally.
     *
     * @param[in] descriptors   A vector where each element is a matrix of descriptors
     *                          (one matrix per image; rows = local descriptors).
     *
     * @return void
     */
    void buildVocabulary(vector<Mat>& descriptors);

    /**
     * @brief Computes the BoVW histogram for a given image descriptor set.
     *
     * This function assigns each descriptor to its nearest visual word in the vocabulary,
     * and creates a normalized histogram representing the frequency of each word.
     *
     * @param[in] descriptors   A matrix of descriptors from an image (rows = local descriptors).
     *
     * @return A normalized histogram (cv::Mat) representing visual word frequencies.
     */
    Mat computeHistogram(const Mat& descriptors);

    /**
     * @brief Returns the current visual vocabulary.
     *
     * @return A matrix (cv::Mat) where each row is a visual word (cluster center).
     */
    Mat getVocabulary() const;

    /**
     * @brief Sets the visual vocabulary manually.
     *
     * Allows overriding or injecting a precomputed vocabulary.
     *
     * @param[in] vocab   A matrix where each row represents a visual word.
     *
     * @return void
     */
    void setVocabulary(Mat vocab);
};
