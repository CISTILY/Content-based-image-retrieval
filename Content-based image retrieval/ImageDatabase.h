#pragma once

#include <opencv2/opencv.hpp>
#include "Image.h"
#include "Logs.h"

using namespace std;
using namespace cv;

/**
 * @brief Manages a collection of images and provides utilities for loading and displaying them.
 *
 * The ImageDatabase class is responsible for reading images from a specified directory,
 * storing them internally, and offering basic utility functions for image access and visualization.
 */
class ImageDatabase {
private:
    vector<Image> Images;  ///< Internal container holding Image objects.

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
     * @brief Loads an image from a given file path and returns it as a Mat object.
     *
     * @param path The full file path to the image.
     * 
     * @return The loaded image as a Mat object.
     */
    Mat loadImageWithPath(string path);

    /**
     * @brief Reads all valid images from the specified directory and stores them in the database.
     *
     * @param imageDatabasePath The path to the folder containing image files.
     * 
     * @param log Logging object to record any issues or progress during loading.
     */
    void readImageDatabase(string imageDatabasePath, Log& log);

    /**
     * @brief Returns the list of Image objects stored in the database.
     *
     * @return A vector of Image objects.
     */
    vector<Image> getImage();

    /**
     * @brief Displays all loaded images in individual OpenCV windows.
     *
     * This method loops through all stored images and opens a window for each one.
     */
    void showAllImg();
};