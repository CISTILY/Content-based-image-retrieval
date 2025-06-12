#pragma once

#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

/**
 * @class Image
 * @brief A wrapper class to represent an image with its ID and content using OpenCV.
 *
 * This class stores both the identifier (typically a file path or name)
 * and the image content itself (`Mat`). It provides utility methods to
 * assign, access, and display the image.
 */
class Image {
private:
    String Id;       ///< Unique identifier or file path of the image.
    Mat content;     ///< Image data (pixel matrix).
public:
    /**
     * @brief Default constructor.
     */
    Image() {}

    /**
     * @brief Default destructor.
     */
    ~Image() {}

    /**
     * @brief Assigns image data and identifier to the Image object.
     *
     * @param id The image identifier (e.g., file name or path).
     * @param img The actual image content as a `Mat`.
     */
    void assignImg(String id, Mat img);

    /**
     * @brief Retrieves the image content.
     *
     * @return The `Mat` object representing the image.
     */
    Mat getImg();

    /**
     * @brief Retrieves the image identifier.
     *
     * @return A string representing the image's ID or file path.
     */
    String getId();

    /**
     * @brief Displays the image in a window using OpenCV.
     */
    void showImg();
};