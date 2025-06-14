#include "ColorCorrelogram.h"

/**
 * Quantization number of bin in a color histogram
 *
 * @param 
 * @return sum of `values`, or 0.0 if `values` is empty.
 */
int ColorCorrelogram::colorQuantization(Vec3b color) {
	int r = color[2] / 64;
	int g = color[1] / 64;
	int b = color[0] / 64;

	return r + g + b;
}

bool ColorCorrelogram::isNeighbor(int x, int y, int rows, int cols) {
    return (x >= 0 && x < rows && y >= 0 && y < cols);
}

void ColorCorrelogram::getNeighborPixels(int x, int y, int rows, int cols, int dist) {
    vector<Point> offsets = {
        {dist,  dist}, {dist, 0}, {dist, -dist}, {0, -dist},
        {-dist, -dist}, {-dist, 0}, {-dist, dist}, {0, dist}
    };
    for (auto& offset : offsets) {
        int nx = x + offset.x;
        int ny = y + offset.y;
        if (isNeighbor(nx, ny, rows, cols)) {
            neighborPixels.push_back(Point(nx, ny));
        }
    }
}

void ColorCorrelogram::createFeature(String image_id, Mat image) {
    vector<int> distances = { 1, 3, 5, 7 };
    const int bins = 64;
    const int rows = image.rows;
    const int cols = image.cols;
    vector<float> correlogram(bins * distances.size(), 0.0f);

    // Step 1: Pre-quantize image
    Mat quantized(rows, cols, CV_8U);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            quantized.at<uchar>(i, j) = colorQuantization(image.at<Vec3b>(i, j));

    // Step 2: Process for each distance
    for (size_t d = 0; d < distances.size(); ++d) {
        int dist = distances[d];
        vector<int> colorCount(bins, 0);
        int totalMatches = 0;

        vector<Point> offsets = {
            { dist,  dist}, { dist, 0}, { dist, -dist}, {0, -dist},
            {-dist, -dist}, {-dist, 0}, {-dist,  dist}, {0,  dist}
        };

        // Sparse sampling every ~100x100 regions
        int stepX = max(1, rows / 100);
        int stepY = max(1, cols / 100);

        for (int x = 0; x < rows; x += stepX) {
            for (int y = 0; y < cols; y += stepY) {
                uchar c1 = quantized.at<uchar>(x, y);

                for (const auto& offset : offsets) {
                    int nx = x + offset.x;
                    int ny = y + offset.y;
                    if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                        uchar c2 = quantized.at<uchar>(nx, ny);
                        if (c1 == c2) {
                            colorCount[c1]++;
                            totalMatches++;
                        }
                    }
                }
            }
        }

        // Normalize
        if (totalMatches > 0) {
            for (int i = 0; i < bins; ++i) {
                correlogram[d * bins + i] = static_cast<float>(colorCount[i]) / totalMatches;
            }
        }
    }

    // Save feature
    id = image_id;
    imageDescriptors = Mat(correlogram).reshape(1, 1); // 1-row descriptor
    imageDescriptors.convertTo(imageDescriptors, CV_32F);
}

void ColorCorrelogram::showFeature(String id) {
    
}