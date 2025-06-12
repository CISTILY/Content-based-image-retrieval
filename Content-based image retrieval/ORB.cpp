#include "ORB.h"

void ORBFeature::createFeature(String id, Mat image) {
    // Convert to grayscale if necessary
    Mat gray;
    if (image.channels() == 3) {
        cvtColor(image, gray, COLOR_BGR2GRAY);
    }
    else {
        gray = image.clone();
    }

    // Create ORB detector
    Ptr<ORB> orb = ORB::create();

    // Detect keypoints and compute descriptors
    vector<KeyPoint> keypoints;
    Mat descriptors;
    orb->detectAndCompute(gray, noArray(), keypoints, descriptors);

    vector<float> descriptorVector;

    // Convert descriptors (uchar matrix) to float vector
    if (!descriptors.empty()) {
        descriptors.convertTo(descriptors, CV_32F);
        //localFeature.assign((float*)descriptors.datastart, (float*)descriptors.dataend);
    }

    //cout << "Extracted " << localFeature.size() << " SIFT descriptor values.\n";

    // Optional: Print first few values
    /*for (size_t i = 0; i < min(size_t(10), localFeature.size()); ++i) {
        cout << localFeature[i] << " ";
    }
    cout << endl;*/
}

void ORBFeature::showFeature(String id) {

}