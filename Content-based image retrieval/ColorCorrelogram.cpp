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
    int bins = 64;
    int rows = image.rows;
    int cols = image.cols;
    vector<float> correlogram(bins * distances.size(), 0.0f);

    for (size_t d = 0; d < distances.size(); ++d) {
        int dist = distances[d];
        vector<int> colorCount(bins, 0);
        int totalMatches = 0;

        for (int x = 0; x < rows; x += max(1, rows / 100)) {
            for (int y = 0; y < cols; y += max(1, cols / 100)) {
                int c1 = colorQuantization(image.at<Vec3b>(x, y));
                getNeighborPixels(x, y, rows, cols, dist);
                for (const auto& pt : neighborPixels) {
                    int c2 = colorQuantization(image.at<Vec3b>(pt.x, pt.y));
                    if (c1 == c2) {
                        colorCount[c1]++;
                        totalMatches++;
                    }
                }
            }
        }

        // Normalize
        for (int i = 0; i < bins; ++i) {
            if (totalMatches > 0) {
                correlogram[d * bins + i] = static_cast<float>(colorCount[i]) / totalMatches;
            }
        }
    }

    //// Combine across distances if needed (here: sum or keep as is)
    //for (int i = 0; i < bins; ++i) {
    //    float result = 0;
    //    for (size_t d = 0; d < distances.size(); ++d) {
    //        result += correlogram[d * bins + i];
    //    }
    //    des.push_back(result);
    //}
    //id = image_id;
}

void ColorCorrelogram::showFeature(String id) {
    
}