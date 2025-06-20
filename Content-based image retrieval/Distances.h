#pragma once

#include <math.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/**
 * @class Distance
 * @brief Provides methods to compute similarity or distance between feature vectors.
 *
 * This class allows the comparison of two feature vectors (e.g., histograms or descriptor-based features)
 * using different distance or similarity metrics such as L2 (Euclidean), Cosine, or others.
 */
class Distance {
public:
    /**
     * @brief Calculates the similarity score between two feature vectors.
     * @param query First feature vector (as a cv::Mat).
     * @param image Second feature vector (as a cv::Mat).
     * @param method Similarity metric to use (e.g., "cosine", "correlation").
     * @return A float similarity score. Higher values mean more similar.
     *
     * This method is typically used when higher scores indicate better matches (e.g., cosine similarity).
     */
    float calculateSimilarity(Mat query, Mat image, string method);

    /**
     * @brief Calculates the distance between two feature vectors.
     * @param query First feature vector (as a cv::Mat).
     * @param image Second feature vector (as a cv::Mat).
     * @param method Distance metric to use (e.g., "euclidean", "chi-squared").
     * @return A float distance score. Lower values mean more similar.
     *
     * This method is used when lower scores indicate better matches (e.g., L2 distance).
     */
    float calculateDistance(Mat query, Mat image, string method);
};
