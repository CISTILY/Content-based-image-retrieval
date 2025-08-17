#pragma once

#include "Features.h"
#include <opencv2/features2d.hpp>

/**
 * @class SIFTFeature
 * @brief A class implementing the SIFT (Scale-Invariant Feature Transform) descriptor extraction.
 *
 * This class inherits from the abstract Feature base class and overrides the createFeature method
 * to extract SIFT descriptors from a given image. The resulting descriptors can be used for
 * local feature matching and Bag-of-Visual-Words indexing.
 */
class SIFTFeature : public Feature {
public:
    /**
     * @brief Default constructor.
     */
    SIFTFeature() {}

    /**
     * @brief Default destructor.
     */
    ~SIFTFeature() {}

    /**
     * @brief Extracts SIFT features from the input image.
     *
     * @param[in] imagePath  Path to the image (used for identification or debugging).
     * @param[in] image      Image data in cv::Mat format to extract SIFT descriptors from.
     *
     * @return void
     *
     * @note The descriptors are stored internally and can be accessed using inherited methods.
     */
    void createFeature(String imagePath, Mat image) override;
};
