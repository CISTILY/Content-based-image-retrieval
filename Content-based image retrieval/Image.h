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
     * @param[in] id   The image identifier (e.g., file name or relative path).
     * @param[in] img  The image data stored as a cv::Mat object.
     *
     * @return void
     */
    void assignImg(String id, Mat img);

    /**
     * @brief Retrieves the image content.
     *
     * @return A cv::Mat representing the pixel data of the image.
     */
    Mat getImg();

    /**
     * @brief Retrieves the image identifier.
     *
     * @return A string representing the image ID or file path.
     */
    String getId();

    /**
     * @brief Displays the image in a new window using OpenCV.
     *
     * @return void
     *
     * @note The function uses `imshow()` and waits for a key press to close.
     */
    void showImg();
};
