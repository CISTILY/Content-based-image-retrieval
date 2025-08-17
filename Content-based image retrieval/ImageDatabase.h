#pragma once

#include <opencv2/opencv.hpp>
#include "Image.h"
#include "Logs.h"

using namespace std;
using namespace cv;

/**
 * @class ImageDatabase
 * @brief Manages a collection of images and provides utilities for loading and displaying them.
 *
 * This class handles reading images from a given directory, storing them as `Image` objects,
 * and offering utility functions to access and display the stored images.
 */
class ImageDatabase {
private:
    vector<Image> Images;  ///< Internal container holding loaded image objects.

public:
    /**
     * @brief Default constructor.
     */
    ImageDatabase() {}

    /**
     * @brief Default destructor.
     */
    ~ImageDatabase() {}

    /**
     * @brief Loads an image from a specified file path.
     *
     * @param[in]  path   Full file path to the image file.
     *
     * @return A cv::Mat object containing the loaded image data.
     *
     * @note This function uses OpenCV’s `imread()` to load the image.
     */
    Mat loadImageWithPath(string path);

    /**
     * @brief Reads all valid image files from a directory and stores them in the internal database.
     *
     * @param[in]      imageDatabasePath   Path to the folder containing image files.
     * @param[in,out]  log                 Logging object used to record success or error messages.
     *
     * @return void
     *
     * @note Supported image formats depend on OpenCV's `imread()` support (e.g., PNG, JPG, BMP).
     *       Invalid or unreadable files are logged via the provided `Log` object.
     */
    void readImageDatabase(string imageDatabasePath, Log& log);

    /**
     * @brief Retrieves the list of stored `Image` objects.
     *
     * @return A vector<Image> containing all loaded images.
     */
    vector<Image> getImage();
};
