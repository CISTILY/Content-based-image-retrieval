#pragma once

#include <opencv2/opencv.hpp>
#include <unordered_set>
#include <string>
#include <vector>

#include "Image.h"
#include "ColorHistogram.h"
#include "ColorCorrelogram.h"
#include "Distances.h"
#include "FeatureDatabase.h"
#include "HOG.h"
#include "ORB.h"
#include "SIFT.h"
#include "BoVW.h"

using namespace std;
using namespace cv;

/**
 * @class Query
 * @brief Handles image query operations and similarity/distance-based retrieval.
 *
 * This class allows performing content-based image retrieval using extracted features
 * and a given index. The query result is returned as a ranked list of image paths and scores.
 */
class Query {
private:
    Image QueryImage;                          ///< Query image metadata and path
    vector<pair<string, float>> results;       ///< Retrieval results (image path, score)
    bool useSimilarity = false;                        ///< Flag to determine whether to use similarity (true) or distance (false)

public:
    /**
     * @brief Executes the query process by comparing the query image to the feature index.
     *
     * @param[in] image_id       The identifier (or path) of the query image.
     * @param[in] query          The feature vector (cv::Mat) of the query image.
     * @param[in] features       Map of image IDs to their corresponding Feature pointers.
     * @param[in] vocabulary     Visual vocabulary used for BoVW-based comparison.
     * @param[in] kTop           The number of top results to retrieve.
     * @param[in] extractMethod  The feature extraction method used (e.g., "SIFT", "ORB", "Color Histogram").
     *
     * @return void
     *
     * @note This function populates the `results` vector with the top-k most similar or closest images.
     */
    void Search(string image_id, Mat query, map<string, Feature*>& features, Mat& vocabulary, int kTop, string extractMethod);

    /**
     * @brief Retrieves the top-k search results after querying.
     *
     * @return A vector of (image path, score) pairs sorted by similarity or distance.
     */
    vector<pair<string, float>> getResult();
};
