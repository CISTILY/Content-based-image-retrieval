#pragma once

#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

/**
 * @class Feature
 * @brief Abstract base class for image feature representation.
 *
 * This class defines the basic interface and structure for all feature extraction methods
 * (e.g., SIFT, Color Histogram, Color Correlogram, etc.). It holds the image ID and the
 * extracted descriptors, and enforces implementation of `createFeature()` in derived classes.
 */
class Feature {
protected:
    String id;                 ///< ID of the image associated with this feature.
    Mat imageDescriptors;      ///< Feature descriptor matrix (e.g., SIFT vectors, histogram).

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
     * @brief Returns the image ID associated with this feature.
     * @return A string ID.
     */
    String getId();

    /**
     * @brief Returns the feature descriptor matrix.
     * @return A const cv::Mat containing the feature descriptors.
     */
    virtual Mat getDescriptor() const { return imageDescriptors; }

    /**
     * @brief Sets the image ID for this feature.
     * @param newId A string representing the new image ID.
     */
    void setId(const string& newId) { id = newId; }

    /**
     * @brief Sets the descriptor matrix for this feature.
     * @param newContent The feature descriptor matrix (e.g., histogram or keypoints).
     */
    void setDescriptor(const Mat& newContent) { imageDescriptors = newContent; }

    /**
     * @brief Pure virtual method to extract and store feature descriptors from an image.
     * @param image_id The ID of the image to associate with the extracted feature.
     * @param image The input image (usually in BGR format).
     *
     * Must be implemented by derived feature classes.
     */
    virtual void createFeature(String image_id, Mat image) = 0;
};
