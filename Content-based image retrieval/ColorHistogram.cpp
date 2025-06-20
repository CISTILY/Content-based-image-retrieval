#include "ColorHistogram.h"

void ColorHistogram::createFeature(String image_id, Mat src_image) {
    // Convert to HSV
    Mat hsv_image;
    cvtColor(src_image, hsv_image, COLOR_BGR2HSV);

    // Split channels
    vector<Mat> hsv_planes;
    split(hsv_image, hsv_planes);

    // Define histogram bin sizes for H, S, V (quantization)
    int h_bins = 16, s_bins = 8, v_bins = 8;
    int histSize[] = { h_bins, s_bins, v_bins };

    // HSV ranges: H [0,180], S,V [0,256]
    float h_range[] = { 0, 180 };
    float s_range[] = { 0, 256 };
    float v_range[] = { 0, 256 };
    const float* ranges[] = { h_range, s_range, v_range };

    // Channel indices: H=0, S=1, V=2
    int channels[] = { 0, 1, 2 };

    // Calculate 3D HSV histogram
    Mat hsv_hist;
    calcHist(&hsv_image, 1, channels, Mat(), hsv_hist, 3, histSize, ranges, true, false);

    // Flatten the 3D histogram to 1D row vector
    hsv_hist = hsv_hist.reshape(1, 1);  // e.g., 1 x (16*8*8) = 1 x 1024

    // Optional: Normalize histogram to [0,1] (or leave raw if you want frequency)
    normalize(hsv_hist, hsv_hist, 0, 1, NORM_MINMAX, -1, Mat());

    // Set attributes
    id = image_id;
    imageDescriptors = hsv_hist;
}