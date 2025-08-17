#pragma once

#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

/**
 * @class Feature
 * @brief Abstract base class for image feature representation.
 *
 * This class defines the standard interface for all feature extraction strategies (e.g., SIFT, Color Histogram, etc.).
 * It stores the image ID and feature descriptor matrix and enforces the implementation of `createFeature()` in derived classes.
 */
class Feature {
protected:
    String id;                ///< The ID of the image associated with this feature.
    Mat imageDescriptors;     ///< The feature descriptor matrix (e.g., SIFT, histogram, etc.).

public:
    /**
     * @brief Default constructor.
     */
    Feature() {}

    /**
     * @brief Virtual destructor.
     */
    virtual ~Feature() {}

    /**
     * @brief Returns the ID of the image associated with this feature.
     *
     * @return A string representing the image ID.
     */
    String getId();

    /**
     * @brief Returns the feature descriptor matrix.
     *
     * @return A cv::Mat representing the extracted feature descriptors.
     */
    virtual Mat getDescriptor() const { return imageDescriptors; }

    /**
     * @brief Sets the ID of the image associated with this feature.
     *
     * @param[in] newId A string representing the image ID.
     * 
     * @return void
     */
    void setId(const string& newId) { id = newId; }

    /**
     * @brief Sets the feature descriptor matrix.
     *
     * @param[in] newContent A cv::Mat representing the new feature descriptors.
     * 
     * @return void
     */
    void setDescriptor(const Mat& newContent) { imageDescriptors = newContent; }

    /**
     * @brief Pure virtual method to extract and assign feature descriptors from an image.
     *
     * This method must be implemented in all derived classes to perform feature extraction.
     *
     * @param[in] image_id The ID of the image to associate with the extracted feature.
     * @param[in] image    The input image (usually in BGR format).
     * 
     * @return void
     */
    virtual void createFeature(String image_id, Mat image) = 0;
};
