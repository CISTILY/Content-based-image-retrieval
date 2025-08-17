#pragma once

#include "Features.h"

/**
 * @class ColorCorrelogram
 * @brief Extracts color correlogram features from an image.
 *
 * The color correlogram encodes how spatial correlation between identical colors
 * varies with distance. This descriptor is robust to changes in image scale,
 * viewpoint, and partial occlusion, and is useful for content-based image retrieval.
 */
class ColorCorrelogram : public Feature {
private:
    vector<Point> neighborPixels; ///< Stores neighbor pixel offsets for a given distance.

public:
    /**
     * @brief Default constructor.
     */
    ColorCorrelogram() {}

    /**
     * @brief Destructor.
     */
    ~ColorCorrelogram() {}

    /**
     * @brief Quantizes a BGR color into a smaller fixed set of bins.
     *
     * @param[in] color   A 3-channel color pixel (BGR format).
     *
     * @return An integer representing the quantized color index.
     */
    int colorQuantization(cv::Vec3b color);

    /**
     * @brief Determines if two pixels are within the specified neighbor radius.
     *
     * @param[in] x1   X-coordinate of the first pixel.
     * @param[in] y1   Y-coordinate of the first pixel.
     * @param[in] x2   X-coordinate of the second pixel.
     * @param[in] y2   Y-coordinate of the second pixel.
     *
     * @return True if the second pixel is within neighbor radius of the first; otherwise false.
     */
    bool isNeighbor(int x1, int y1, int x2, int y2);

    /**
     * @brief Computes valid neighbor pixel offsets at a given distance.
     *
     * Generates the relative coordinate offsets for all pixels at the specified radius
     * that lie within the image bounds.
     *
     * @param[in] width     Width of the image.
     * @param[in] height    Height of the image.
     * @param[in] x         X-coordinate of the center pixel.
     * @param[in] y         Y-coordinate of the center pixel.
     * @param[in] distance  Radius at which to search for neighbors.
     *
     * @return void
     */
    void getNeighborPixels(int width, int height, int x, int y, int distance);

    /**
     * @brief Extracts the color correlogram feature for an input image.
     *
     * This method computes spatial correlations of identical quantized colors
     * at predefined distances from each pixel in the image and stores the result
     * in the internal feature descriptor.
     *
     * @param[in] image_id   A unique string identifier for the image.
     * @param[in] src_image  The input image in BGR format.
     *
     * @return void
     */
    void createFeature(cv::String image_id, cv::Mat src_image) override;
};
