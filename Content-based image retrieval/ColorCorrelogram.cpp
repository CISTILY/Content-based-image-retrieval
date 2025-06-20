#include "ColorCorrelogram.h"

int ColorCorrelogram::colorQuantization(Vec3b hsvColor) {
    int h = hsvColor[0]; // H: 0-179
    int s = hsvColor[1]; // S: 0-255
    int v = hsvColor[2]; // V: 0-255

    int h_bin = h / 20;  // 9 bins (0–8)
    int s_bin = s / 64;  // 4 bins (0–3)
    int v_bin = v / 64;  // 4 bins (0–3)

    // Unique bin index: h ∈ [0,8], s ∈ [0,3], v ∈ [0,3]
    return h_bin * (4 * 4) + s_bin * 4 + v_bin; // total: 144 bins
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
    const int h_bins = 9, s_bins = 4, v_bins = 4;
    const int bins = h_bins * s_bins * v_bins; // total = 144
    const int rows = image.rows;
    const int cols = image.cols;
    vector<float> correlogram(bins * distances.size(), 0.0f);

    // Convert to HSV and quantize
    Mat hsv_image;
    cvtColor(image, hsv_image, COLOR_BGR2HSV);

    Mat quantized(rows, cols, CV_8U);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            quantized.at<uchar>(i, j) = colorQuantization(hsv_image.at<Vec3b>(i, j));

    // Step 2: Compute correlogram
    for (size_t d = 0; d < distances.size(); ++d) {
        int dist = distances[d];
        vector<int> colorCount(bins, 0);
        int totalMatches = 0;

        vector<Point> offsets = {
            { dist,  dist}, { dist, 0}, { dist, -dist}, {0, -dist},
            {-dist, -dist}, {-dist, 0}, {-dist,  dist}, {0,  dist}
        };

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

        if (totalMatches > 0) {
            for (int i = 0; i < bins; ++i) {
                correlogram[d * bins + i] = static_cast<float>(colorCount[i]) / totalMatches;
            }
        }
    }

    // Save result
    id = image_id;
    imageDescriptors = Mat(correlogram).reshape(1, 1); // 1-row descriptor
    imageDescriptors.convertTo(imageDescriptors, CV_32F);
}