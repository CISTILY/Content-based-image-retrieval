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
 * using different distance or similarity metrics such as L2 (Euclidean), Cosine similarity, or correlation.
 */
class Distance {
public:
    /**
     * @brief Calculates the similarity score between two feature vectors.
     *
     * This method computes how similar two feature vectors are using the specified similarity metric.
     * Typically used in applications where higher values represent higher similarity.
     *
     * @param[in] query   First feature vector (query) represented as a cv::Mat.
     * @param[in] image   Second feature vector (from image database) as a cv::Mat.
     * @param[in] method  The similarity metric to use (e.g., "cosine", "correlation").
     *
     * @return A float representing the similarity score. Higher values indicate higher similarity.
     */
    float calculateSimilarity(Mat query, Mat image, string method);

    /**
     * @brief Calculates the distance between two feature vectors.
     *
     * This method measures how different two feature vectors are using the specified distance metric.
     * Used in applications where lower values indicate better matches (e.g., L2 or Chi-Squared).
     *
     * @param[in] query   First feature vector (query) represented as a cv::Mat.
     * @param[in] image   Second feature vector (from image database) as a cv::Mat.
     * @param[in] method  The distance metric to use (e.g., "euclidean", "chi-squared").
     *
     * @return A float representing the distance score. Lower values indicate higher similarity.
     */
    float calculateDistance(Mat query, Mat image, string method);
};
