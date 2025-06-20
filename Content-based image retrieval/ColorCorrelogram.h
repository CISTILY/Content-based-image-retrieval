#pragma once

#include "Features.h"

/**
 * @class ColorCorrelogram
 * @brief Extracts color correlogram features from an image.
 *
 * A color correlogram captures the spatial correlation of colors,
 * encoding the probability of finding a color at a certain distance
 * from a given pixel of the same color. It is useful for robust image retrieval.
 */
class ColorCorrelogram : public Feature {
private:
    std::vector<cv::Point> neighborPixels; ///< Stores neighbor pixel offsets for given radius.

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
     * @brief Quantizes an RGB color to a reduced number of bins.
     * @param color A 3-channel pixel value (BGR).
     * @return An integer representing the quantized color index.
     */
    int colorQuantization(cv::Vec3b color);

    /**
     * @brief Checks if two pixels are neighbors based on a given distance.
     * @param x1 First pixel x-coordinate.
     * @param y1 First pixel y-coordinate.
     * @param x2 Second pixel x-coordinate.
     * @param y2 Second pixel y-coordinate.
     * @return True if the two pixels are within the specified neighbor radius.
     */
    bool isNeighbor(int x1, int y1, int x2, int y2);

    /**
     * @brief Computes relative neighbor pixel offsets for a given distance.
     * @param width Image width.
     * @param height Image height.
     * @param x Current pixel x-coordinate.
     * @param y Current pixel y-coordinate.
     * @param distance Distance (radius) to search for neighbors.
     */
    void getNeighborPixels(int width, int height, int x, int y, int distance);

    /**
     * @brief Extracts and stores the color correlogram feature from an image.
     * @param image_id A string ID associated with the image.
     * @param src_image The input image (BGR color format).
     */
    void createFeature(cv::String image_id, cv::Mat src_image) override;
};
