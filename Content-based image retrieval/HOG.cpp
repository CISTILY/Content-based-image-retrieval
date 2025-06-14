#include "HOG.h"

void HOG::createFeature(String image_id, Mat src_image) {
    // Convert to grayscale if it's a color image
    Mat gray;
    if (src_image.channels() == 3) {
        cvtColor(src_image, gray, COLOR_BGR2GRAY);
    }
    else {
        gray = src_image.clone();
    }

    // Ensure the image type is CV_8U
    if (gray.type() != CV_8U) {
        gray.convertTo(gray, CV_8U);
    }

    // Define HOG descriptor
    HOGDescriptor hog(
        Size(64, 128), // winSize
        Size(16, 16),  // blockSize
        Size(8, 8),    // blockStride
        Size(8, 8),    // cellSize
        9              // nbins
    );

    // Resize input if it's smaller than winSize (HOG requires full blocks to fit)
    if (gray.cols < hog.winSize.width || gray.rows < hog.winSize.height) {
        resize(gray, gray, hog.winSize);
    }

    // Compute HOG descriptor
    std::vector<float> descriptors;
    hog.compute(gray, descriptors);

    // Convert to cv::Mat
    Mat hogDescriptor = Mat(descriptors).clone();

    // Save results
    id = image_id;
    imageDescriptors = hogDescriptor;
}

void HOG::showFeature(String id) {

}