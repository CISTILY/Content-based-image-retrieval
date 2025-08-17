#pragma once

#include "Features.h"
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

/**
 * @class HOG
 * @brief Extracts and manages HOG (Histogram of Oriented Gradients) features for image representation.
 *
 * Inherits from the abstract base class `Feature`. This class is responsible for extracting
 * HOG descriptors from input images, enabling them to be used in indexing, clustering,
 * and retrieval processes.
 */
class HOG : public Feature {
public:
    /**
     * @brief Extracts HOG features from an image.
     *
     * Overrides the base class method to compute HOG descriptors and store them internally.
     *
     * @param[in] imagePath   The path or ID of the image being processed.
     * @param[in] image       The input image (grayscale or BGR).
     *
     * @return void
     *
     * @note The resulting descriptors are stored in `imageDescriptors` from the base class.
     */
    void createFeature(String imagePath, Mat image) override;

    /**
     * @brief Computes the HOG descriptor from a given image.
     *
     * @param[in]  image       The input image for which HOG features are computed.
     * @param[in]  mask        Optional mask (not used in standard HOG, but included for compatibility).
     * @param[in]  visualize   Flag to indicate whether a visualization is to be generated (default false).
     *
     * @return A cv::Mat containing the 1D HOG descriptor vector.
     *
     * @note This function supports downstream use in Bag-of-Visual-Words and similarity comparison.
     */
    Mat computeHOG(InputArray image, InputArray mask, bool visualize);
};
