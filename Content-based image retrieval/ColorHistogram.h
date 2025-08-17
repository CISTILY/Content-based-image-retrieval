#pragma once

#include "Features.h"
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

/**
 * @class ColorHistogram
 * @brief Extracts and stores color histogram features from an image.
 *
 * This class implements a color-based global feature descriptor by computing
 * separate 1D histograms for the Blue, Green, and Red channels in an image.
 * The extracted histogram can be used for color-based image retrieval and comparison.
 */
class ColorHistogram : public Feature {
private:
    Mat r_hist;   ///< Histogram of the Red channel.
    Mat g_hist;   ///< Histogram of the Green channel.
    Mat b_hist;   ///< Histogram of the Blue channel.

public:
    /**
     * @brief Extracts and stores the RGB color histograms from a source image.
     *
     * This method computes a separate histogram for each color channel (B, G, R),
     * typically using 256 bins per channel. The resulting histograms are concatenated
     * and stored as the descriptor for the image.
     *
     * @param[in] image_id     A unique string identifier for the input image.
     * @param[in] src_image    The input image in BGR color space (cv::Mat format).
     *
     * @return void
     */
    void createFeature(String image_id, Mat src_image) override;
};
