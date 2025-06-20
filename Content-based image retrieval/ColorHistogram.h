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
 * This class implements a simple RGB color histogram representation by computing
 * separate histograms for the Red, Green, and Blue channels of an image.
 */
class ColorHistogram : public Feature {
private:
    Mat r_hist;  ///< Histogram of the Red channel.
    Mat g_hist;  ///< Histogram of the Green channel.
    Mat b_hist;  ///< Histogram of the Blue channel.

public:
    /**
     * @brief Extracts and stores the RGB color histogram from the given image.
     * @param image_id A string ID used to identify the image.
     * @param src_image The input image (in BGR format).
     *
     * This method computes 1D histograms for each channel (typically with 256 bins),
     * which can later be used for color-based image retrieval or comparison.
     */
    void createFeature(String image_id, Mat src_image) override;
};
