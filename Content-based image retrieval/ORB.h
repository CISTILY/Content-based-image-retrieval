#pragma once

#include "Features.h"

/**
 * @class ORBFeature
 * @brief A class that implements the ORB (Oriented FAST and Rotated BRIEF) feature extraction method.
 *
 * Inherits from the abstract Feature class and overrides the createFeature() method
 * to extract ORB descriptors from a given image.
 */
class ORBFeature : public Feature {
public:
    /**
     * @brief Default constructor.
     */
    ORBFeature() {}

    /**
     * @brief Default destructor.
     */
    ~ORBFeature() {}

    /**
     * @brief Extracts ORB features from a given image and stores them in the object.
     *
     * @param[in] imagePath  Path to the image file.
     * @param[in] image      The image matrix (cv::Mat) from which to extract features.
     *
     * @return void
     *
     * @note The descriptors are stored internally and can be accessed using inherited methods.
     */
    void createFeature(String imagePath, Mat image) override;
};
